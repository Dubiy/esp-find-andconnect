void showConnectionInfo() {
  Serial.print("MAC Addr: ");
  Serial.println(WiFi.macAddress());
  Serial.print("IP Addr:  ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet:   ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway:  ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS Addr: ");
  Serial.println(WiFi.dnsIP());
  Serial.print("Channel:  ");
  Serial.println(WiFi.channel());
  Serial.print("Status: ");
  Serial.println(WiFi.status());
}

void setupAp() {
  WiFi.mode(WIFI_AP);
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)) ? "Ready" : "Failed!");

  String apSsid = "uworld-node-" + WiFi.softAPmacAddress();
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(apSsid.c_str()) ? "Ready" : "Failed!");

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
}
