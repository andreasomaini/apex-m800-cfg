#!/usr/bin/python3

'''
* This script converts an exported JSON file from Wireshark and extracts the
* binary data into a new file that the uploader program can easily read.
'''


from sys import argv
from json import loads, JSONDecodeError

PACKET_SIZE = 514
PACKETS_NUMBER = 6

if len(argv) != 3:
    print("Usage: {0} </path/to/json_file> </path/to/output>".format(argv[0]))
    quit(1)

try:
    with open(argv[1], 'r') as fp:
        try:
            json_obj = loads(fp.read())
        except:
            print("{0}: Couldn't parse input file correctly. Is it really a json file?".format(
                argv[0]))
            quit(1)
except:
    print("{0}: Couldn't open input file {1}".format(argv[0], argv[1]))
    quit(1)

# Open the output file and read the binaries from the json
try:
    with open(argv[2], 'wb') as output_file:
        # Sanity check: there should be 6 packets, 514 of size each
        if len(json_obj) != PACKETS_NUMBER:
            print("{0}: [WARNING] Sanity check failed, found {1} packets instead of {2}".format(
                argv[0], len(json_obj), PACKETS_NUMBER))
        for packet in json_obj:
            data = packet['_source']['layers']["URB setup"]["usb.data_fragment"]
            bytes_data = bytearray([int(single_byte, 16)
                                    for single_byte in data.split(':')])

            # Sanity check for the packet size
            if len(bytes_data) != PACKET_SIZE:
                print("{0}: [WARNING] Sanity check failed, found a packet of size {1} instead of {2}".format(
                    argv[0], len(bytes_data), PACKET_SIZE))
            output_file.write(bytes_data)
except:
    print("{0}: Couldn't open output file {1}".format(argv[0], argv[2]))
    quit(1)

print("{0}: Successfully written binary data in {1}".format(argv[0], argv[2]))
