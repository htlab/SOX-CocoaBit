#ifndef _H_THIN_SOX_CLIENT
#define _H_THIN_SOX_CLIENT

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <WiFiClient.h>
//#include <StringUtil.h>
//using namespace StringUtil;
#include <Time.h>
#include <TimeLib.h>
#include <Nefry.h>

class ThinSOXClient {
  private:
    WiFiClient client;
    char server_hostname[128];
    int server_port;

    int openStream(char *server);
    int authenticate(char *username, char *password);
    int bindResource(char *resource);
    int openSession(char *server);
    int authenticateWithAnonymous();

    int sendTemplate(const prog_char *strTemplate, int fillLen, ...);

  public:
    ThinSOXClient();
    //ThinSOXClient(String *hostname, uint16_t port);

    int connect(String server, int port);
    int publish(String nodename, String transducer_id, String timestamp, String typedvalue, String rawvalue);
    int close();

};

#endif /* _H_THIN_SOX_CLIENT */
