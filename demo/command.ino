void getNextCommand() {
  // close any connection before send a new request
  client.stop();

  Serial.print("Getting next command from ");
  Serial.print(server);
  Serial.println("...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 8090)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());

    // Make a HTTP request:
    client.println("POST /commands/dequeue HTTP/1.1");
    client.println("Host: 192.168.0.3:8090");
    client.println("0");
    client.println("Accept: text/plain");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made
    lastCommandExecutionTime = millis();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

COMMAND readCommand(void) {
  static String serialData = "";
  while ( client.available() > 0 ) {
    byte buffer[8];
    client.read(buffer, 1);

    char character = buffer[0];
    serialData += character;

    // Return with no command if no end of line reached
    if (character != '\n') continue;

    // End of line reached, process command
    if ( serialData.equals("COMMAND_COOLING_ON\n") )
    {
      Serial.println("COMMAND_COOLING_ON reached");
      serialData = "";
      return COMMAND_COOLING_ON;
    }
    else if (serialData.equals("COMMAND_COOLING_OFF\n"))
    {
      Serial.println("COMMAND_COOLING_OFF reached");
      serialData = "";
      return COMMAND_COOLING_OFF;
    }
    else
    {
      serialData = "";
      return COMMAND_NONE;
    }
  }
  return COMMAND_NONE;
}

void executeCommand(COMMAND command) {

  if (command == COMMAND_COOLING_ON) {
    Serial.println("turning colling on");
    digitalWrite(COOLING, HIGH);
  } else if (command == COMMAND_COOLING_OFF) {
    Serial.println("turning colling off");
    digitalWrite(COOLING, LOW);
  }
  
  return;
}
