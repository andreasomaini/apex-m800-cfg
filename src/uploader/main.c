#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <libusb-1.0/libusb.h>

#define VENDOR_ID 0x1038
#define PRODUCT_ID 0x1600

#define PAYLOAD_NUMBER 6
#define PAYLOAD_SIZE 514

#define LAST_PAYLOAD_SIZE 32

// Checks whether *device* is the keyboard
int isKeyboard(libusb_device *device);

// Needs as first argument the path of the payload file
int main(int argc, char **argv) {
  // Check for command line argument
  if (argc != 2) {
    fprintf(stderr, "Usage: %s </path/to/payload>\n", argv[0]);
    return 1;
  }

  // Try to open file
  FILE *payloadFile = fopen(argv[1], "rb");

  if (!payloadFile) {
    fprintf(stderr, "%s: Unable to read payload file: %s\n", argv[0],
            strerror(errno));
    return errno;
  }

  // Initialize libusb
  libusb_context *context;

  if (libusb_init(&context)) {
    fprintf(stderr, "%s: Unable to initialize libusb\n", argv[0]);
    return 1;
  }

  // Look for the keyboard in the USB devices list
  libusb_device **deviceList;

  int totalDevice = libusb_get_device_list(context, &deviceList);

  int i;
  for (i = 0; i < totalDevice; i++) {
    if (isKeyboard(deviceList[i]))
      break;
  }

  if (i == totalDevice) {
    fprintf(stderr, "%s: Keyboard not found :(\n", argv[0]);
    return 1;
  }

  // Now, open the device
  libusb_device_handle *keyboardHandle = NULL;

  if (libusb_open(deviceList[i], &keyboardHandle) != LIBUSB_SUCCESS) {
    fprintf(stderr, "%s: Error during device opening\n", argv[0]);
    return 1;
  }

  // Detach the kernel driver and claim the interface
  libusb_set_auto_detach_kernel_driver(keyboardHandle, 1);
  if (libusb_claim_interface(keyboardHandle, 0) != 0) {
    fprintf(stderr, "%s: Unable to claim the keyboard interface\n", argv[0]);
    return 1;
  }

  // Start sending the payloads
  unsigned char payload[PAYLOAD_SIZE];

  for (i = 0; i < PAYLOAD_NUMBER; i++) {
    // Read the payload from the file
    fread(payload, PAYLOAD_SIZE, 1, payloadFile);

    // Send the payload via USB
    int bytesSent = libusb_control_transfer(
        keyboardHandle,
        0x21,         // bMRequest Type
        9,            // bRequest
        0x0300,       // wValue : 0x0200 for the update
                      //          0x0300 for the payloads upload
        0,            // wIndex,
        payload,      // Data
        PAYLOAD_SIZE, // wLenght,
        0             // Wait forever
    );

    // Check the upload went fine
    if (bytesSent != PAYLOAD_SIZE) {
      fprintf(stderr, "%s: Unable to send payload n.%d: %s\n", argv[0], i,
              strerror(errno));
      // Free resources
      fclose(payloadFile);
      libusb_release_interface(keyboardHandle, 0);
      libusb_close(keyboardHandle);
      libusb_free_device_list(deviceList, 1);
      libusb_exit(context);
      return errno;
    }
  }

  // Update the keyboard
  unsigned char lastPayload[LAST_PAYLOAD_SIZE] = {0};
  lastPayload[0] = 0x09;

  int bytesSent =
      libusb_control_transfer(keyboardHandle,
                              0x21,   // bMRequest Type
                              9,      // bRequest
                              0x0200, // wValue : 0x0200 for the update
                                      //          0x0300 for the payloads upload
                              0,      // wIndex,
                              lastPayload,       // Data
                              LAST_PAYLOAD_SIZE, // wLenght,
                              0                  // Wait forever
      );

  // Check the update went fine
  if (bytesSent != LAST_PAYLOAD_SIZE) {
    fprintf(stderr, "%s: Unable to send last payload: %s\n", argv[0],
            strerror(errno));
    // Free resources
    libusb_release_interface(keyboardHandle, 0);
    libusb_close(keyboardHandle);
    libusb_free_device_list(deviceList, 1);
    libusb_exit(context);
    fclose(payloadFile);
    return errno;
  }

  // Free resources
  libusb_release_interface(keyboardHandle, 0);
  libusb_close(keyboardHandle);
  libusb_free_device_list(deviceList, 1);
  libusb_exit(context);

  fclose(payloadFile);

  return 0;
}

int isKeyboard(libusb_device *device) {

  struct libusb_device_descriptor descriptor;

  if (libusb_get_device_descriptor(device, &descriptor) < 0) {
    return 0;
  }

  if (descriptor.idVendor == VENDOR_ID && descriptor.idProduct == PRODUCT_ID) {
    return 1;
  }

  return 0;
}