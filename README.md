# RPIPicoWExp-Mongoose
Experiments with Mongoose Web Server on RPI Pico W. 

This repo contains examples projects that I talk through on my YouTube channel [drjonea](https://youtube.com/@drjonea).

## Cloning and Building

The repo uses submodules for libraries so please close with the --recurse-submodules switch.

The examples all use WiFi and therefore need your Wifi SSID and Password.  You can either edit the "src/CMakelists.txt"
in each example to provide this. Change the lines shown below:
```
target_compile_definitions(${NAME} PRIVATE
    WIFI_SSID=\"$ENV{WIFI_SSID}\"
    WIFI_PASS=\"$ENV{WIFI_PASSWORD}\"
)
```
Or as written this will try to pull the SSID and Password from the environmental variables:
+ WIFI_SSID
+ WIFI_PASSWORD

You should be able to import this project into VSCode and compile with the CMake extension tool. Though I personally
have compiled it using a stand alone toolchain and the following build process.

```
cd projectFolder
mkdir build
cmake ..
make # or ninja
```
Then flash the firmware with picotool, openocd or bootsel process. 

## Examples

### StaticBaremetal
This is an example to serve static html files from Mongoose in baremetal mode. It uses Mongoose own TCPIP stack and Pico W drivers.
The static webpages are in the "Web" folder. I then manual use the "pack" tool from Mongoose to convert these to a filesystem data
structure in the fs.c file. See the video

The example listens to HTTP requests on port 80.

### StaticFreeRTOS
This is the same example as Baremetal but using the LWIP TCPIP stack and Pico SDK Pico W Driver. To do that we need to also use
FreeRTOS Kernel so we have TCPIP Sockets available.

### WifiScanner
This example uses Mongoose to show the results of a Wifi Scan for Wifi Networks running on the Pico W.



