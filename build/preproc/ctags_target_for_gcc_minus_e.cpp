# 1 "C:\\repos\\rtos_esp32c6\\rtos_esp32c6.ino"
/*

 *  This sketch demonstrates how to scan WiFi networks.

 *  The API is based on the Arduino WiFi Shield library, but has significant changes as newer WiFi functions are supported.

 *  E.g. the return value of `encryptionType()` different because more modern encryption is supported.

 */
# 6 "C:\\repos\\rtos_esp32c6\\rtos_esp32c6.ino"
# 7 "C:\\repos\\rtos_esp32c6\\rtos_esp32c6.ino" 2

void setup()
{
    HWCDCSerial.begin(115200);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected.
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();
    delay(100);

    HWCDCSerial.println("Setup done");
}

void loop()
{
    HWCDCSerial.println("Scan start");

    // WiFi.scanNetworks will return the number of networks found.
    int n = WiFi.scanNetworks();
    HWCDCSerial.println("Scan done");
    if (n == 0) {
        HWCDCSerial.println("no networks found");
    } else {
        HWCDCSerial.print(n);
        HWCDCSerial.println(" networks found");
        HWCDCSerial.println("Nr | SSID                             | RSSI | CH | Encryption");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            HWCDCSerial.printf("%2d",i + 1);
            HWCDCSerial.print(" | ");
            HWCDCSerial.printf("%-32.32s", WiFi.SSID(i).c_str());
            HWCDCSerial.print(" | ");
            HWCDCSerial.printf("%4ld", WiFi.RSSI(i));
            HWCDCSerial.print(" | ");
            HWCDCSerial.printf("%2ld", WiFi.channel(i));
            HWCDCSerial.print(" | ");
            switch (WiFi.encryptionType(i))
            {
            case WIFI_AUTH_OPEN:
                HWCDCSerial.print("open");
                break;
            case WIFI_AUTH_WEP:
                HWCDCSerial.print("WEP");
                break;
            case WIFI_AUTH_WPA_PSK:
                HWCDCSerial.print("WPA");
                break;
            case WIFI_AUTH_WPA2_PSK:
                HWCDCSerial.print("WPA2");
                break;
            case WIFI_AUTH_WPA_WPA2_PSK:
                HWCDCSerial.print("WPA+WPA2");
                break;
            case WIFI_AUTH_WPA2_ENTERPRISE:
                HWCDCSerial.print("WPA2-EAP");
                break;
            case WIFI_AUTH_WPA3_PSK:
                HWCDCSerial.print("WPA3");
                break;
            case WIFI_AUTH_WPA2_WPA3_PSK:
                HWCDCSerial.print("WPA2+WPA3");
                break;
            case WIFI_AUTH_WAPI_PSK:
                HWCDCSerial.print("WAPI");
                break;
            default:
                HWCDCSerial.print("unknown");
            }
            HWCDCSerial.println();
            delay(10);
        }
    }
    HWCDCSerial.println("");

    // Delete the scan result to free memory for code below.
    WiFi.scanDelete();

    // Wait a bit before scanning again.
    delay(5000);
}
