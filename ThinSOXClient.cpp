#include "ThinSOXClient.h"

/****************/
/* ThinSOX STANZAS */
/***************/
//Stanza (1) Opening Stream (used)
static const prog_char PROGMEM open_stream_template[] = "<stream:stream "
    "xmlns='jabber:client' "
    "xmlns:stream='http://etherx.jabber.org/streams' "
    "to='%s' "
    "xml:lang='en' "
    "version='1.0'>";
//s = hostname

//Stanza (2) Anonymous Auth (used)
static const prog_char PROGMEM anonymous_auth_template[] = "<auth "
    "xmlns='urn:ietf:params:xml:ns:ThinSOX-sasl' "
    "mechanism='ANONYMOUS'>"
    "="
    "</auth>";

//Stanza (3) Plain Auth (NOT used)
static const prog_char PROGMEM plain_auth_template[] = "<auth "
    "xmlns='urn:ietf:params:xml:ns:ThinSOX-sasl' "
    "mechanism='PLAIN'>"
    "%s"
    "</auth>";

//Stanza (4) Bind (used)
//<iq id='IelJO-4' type='set'><bind xmlns='urn:ietf:params:xml:ns:ThinSOX-bind'></bind></iq>
static const prog_char PROGMEM bind_template[] = "<iq "
    "type='set' "
    "id='bind_1'>"
    "<bind "
    "xmlns='urn:ietf:params:xml:ns:ThinSOX-bind'>"
    "<resource>%s</resource>"
    "</bind>"
    "</iq>";

//Stanza (5) Request (NOT used)
static const prog_char PROGMEM session_request_template[] = "<iq "
    "to='%s' "
    "type='set' "
    "id='ard_sess'>"
    "<session "
    "xmlns='urn:ietf:params:xml:ns:ThinSOX-session' />"
    "</iq>";

//Stanza (6) Presence (NOT used)
static const prog_char PROGMEM presence_template[] = "<presence>"
    "<show/>"
    "</presence>";

//Stanza (7) Message (NOT used)
static const prog_char PROGMEM message_template[] = "<message "
    "to='%s' "
    "xmlns='jabber:client' "
    "type='chat' "
    "id='msg' "
    "xml:lang='en'>"
    "<body>%s</body>"
    "</message>";

//Stanza (8) SOX Publish (used)
static const prog_char PROGMEM SOXpublish_template[] =  "<iq to='%s' id='SOxPublish-1' type='set'>"
    "<pubsub xmlns='http://jabber.org/protocol/pubsub'>"
    "<publish node='%s'>"
    "<item>"
    "<data>"
    "<transducerValue id='%s' "
    "timestamp='%s' "
    "typedValue='%s' "
    "rawValue='%s' />"
    "</data>"
    "</item>"
    "</publish>"
    "</pubsub>"
    "</iq>";

//Stanza (9) Close (used)
static const prog_char PROGMEM close_template[] = "<presence type='unavailable'/>"
    "</stream:stream>";


/////////////////////////////////////////////////////////////////////////////////////
ThinSOXClient::ThinSOXClient() {
  ;
}

//ThinSOXClient::ThinSOXClient(char *server, uint16_t port) : client(server, port) {
//    ;
//}

/////////////////////////////////////////////////////////////////////////////////////
int ThinSOXClient::connect(String in_servername, int in_port) {

  boolean connected = false, error = false;

  bzero(this->server_hostname, 128);
  strcpy(this->server_hostname, in_servername.c_str());
  this->server_port = in_port;
  Nefry.println(String("ThinSOXClient::connect host=[") + in_servername + "]");
  Nefry.println(String("ThinSOXClient::connect host=[") + this->server_hostname + "]");
  Nefry.println(String("ThinSOXClient::connect port=[") + this->server_port + "]");
  
  if (!client.connect(this->server_hostname, this->server_port)) {
    /* Retry connection every 1s and block until we're successful */
    Nefry.println("connection failed");
    return (-1);
  }

  //openstream
  //if(openStream(this->server_hostname) <0){
  //  return(-1);
  //}
  client.print(String("<stream:stream xmlns='jabber:client' to='sox.ht.sfc.keio.ac.jp' xmlns:stream='http://etherx.jabber.org/streams' version='1.0' xml:lang='en'>"));
  
  //auth
  //if(authenticateWithAnonymous()<0){
  //  return(-1);
  //}
  client.print(String("<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='ANONYMOUS'>=</auth>"));
    
  //bind
  //if(bindResource("")<0){
  //  return(-1);
  //}
  client.print(String("<iq id='IelJO-4' type='set'><bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'></bind></iq>"));
  
  client.flush();
  Nefry.println("ThinSOXClient:: successfully finished.");
  return (1);
}


int ThinSOXClient::publish(String nodename, String transducer_id, String timestamp, String typedvalue, String rawvalue) {
  Nefry.println("publish: start.");
  Nefry.println(String("publish: nodename: ") + nodename);
  Nefry.println(String("publish: transducerID: ") + transducer_id);
  Nefry.println(String("publish: timestamp: ") + timestamp);
  Nefry.println(String("publish: typedvalue: ") + typedvalue);
  Nefry.println(String("publish: rawvalue: ") + rawvalue);

  //Following is code to publish sensor data.
  //Ideally, this code should be done by using "sendTemplate" function.
  //Following current code looks very ugly, but is a quick dirty hack...
  client.print(String("<iq to='pubsub.") + String(this->server_hostname)  + String("' id='IelJO-15' type='set'><pubsub xmlns='http://jabber.org/protocol/pubsub'><publish node='") + nodename + String("'><item><data><transducerValue id='") + transducer_id + String("' typedValue='") + rawvalue + String("' timestamp='") + timestamp + String("' rawValue='") + rawvalue + String("'/></data></item></publish></pubsub></iq>"));
  client.flush();
  client.stop();

  /*sendTemplate(SOXpublish_template,
              strlen(this->server_hostname) + strlen(nodename) + strlen(transducer_id) + strlen(timestamp) + strlen(typedvalue) + strlen(rawvalue),
              this->server_hostname,
              nodename,
              transducer_id,
              timestamp,
              typedvalue,
              rawvalue);*/

  Nefry.println("publish: print...done.");
  return (1);
}

int ThinSOXClient::close() {
  sendTemplate(close_template, 0);
}

/////////////////////////
int ThinSOXClient::sendTemplate(const prog_char *temp_P, int fillLen, ...) {
  int tempLen = strlen_P(temp_P);
  char temp[tempLen];
  char buffer[tempLen + fillLen];
  va_list args;

  strcpy_P(temp, temp_P);

  va_start(args, fillLen);
  //StringUtil::vsprintf(buffer, temp, args);

  Nefry.println("mesg = [" + String(temp) + "]");
  Nefry.println("mesg = [" + String(buffer) + "]");

  //client.print(buffer);
  client.write(buffer);

  return 1;
}

/////////////////////////////////////
int ThinSOXClient::openStream(char *server) {
  sendTemplate(open_stream_template, strlen(server), server);
}

//Not used
int ThinSOXClient::openSession(char *server) {
  sendTemplate(session_request_template, strlen(server), server);
}

int ThinSOXClient::authenticateWithAnonymous() {
  sendTemplate(anonymous_auth_template, 0, "");
}

int ThinSOXClient::bindResource(char *resource) {
  sendTemplate(bind_template, strlen(resource), resource);
}

//Not used
//int ThinSOXClient::sendMessage(char *recipientJid, char *message) {
//  sendTemplate(message_template, strlen(recipientJid) + strlen(message), recipientJid, message);
//}
