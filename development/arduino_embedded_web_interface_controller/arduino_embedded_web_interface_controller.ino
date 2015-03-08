/*
 *  Arduino Embedded Web Interface Controller.
 *
 *  Copyright (C) 2010 Efstathios Chatzikyriakidis (contact@efxa.org)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// include related libraries for ethernet and webduino web server.
#include <SPI.h>
#include <Ethernet.h>
#include <WebServer.h>

// data type definition for an arduino PIN.
typedef struct PIN_TYPE {
  int num; // the number of the PIN.
  int val; // the value on the PIN.
} PIN_TYPE;

// start arduino board architecture data.

// PINS description (default: duemilanove board).

// array for the digital standard logic (LOW / HIGH) PINS.
PIN_TYPE DIGITAL_OUT_STDLOGIC_PINS[] = {
  { 0, LOW }, { 1, LOW },
  { 2, LOW }, { 3, LOW },
  { 4, LOW }, { 5, LOW },
  { 6, LOW }, { 7, LOW },
  { 8, LOW }, { 9, LOW }
};

// array for the digital PWM PINS.
PIN_TYPE DIGITAL_OUT_PWM_PINS[] = {
  { 3, LOW },
  { 5, LOW },
  { 6, LOW },
  { 9, LOW }
};

// array for the analog input PINS.
const int ANALOG_INPUT_PINS[] = {
  0, 1, 2, 3, 4, 5
};

// end arduino board architecture data.

// number of the digital standard logic PINS.
const int MAX_DIGITAL_STDLOGIC_OUTS =
  (int) (sizeof (DIGITAL_OUT_STDLOGIC_PINS) / sizeof (DIGITAL_OUT_STDLOGIC_PINS[0]));

// number of the digital PWM PINS.
const int MAX_DIGITAL_PWM_OUTS =
  (int) (sizeof (DIGITAL_OUT_PWM_PINS) / sizeof (DIGITAL_OUT_PWM_PINS[0]));

// number of the analog input PINS.
const int MAX_ANALOG_INPUTS =
  (int) (sizeof (ANALOG_INPUT_PINS) / sizeof (ANALOG_INPUT_PINS[0]));

// no-cost printing stream operator.
template<class T>
inline Print & operator << (Print & obj, T arg) {
  obj.print(arg);
  return obj;
}

// mac address for the arduino ethernet shield.
static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// ip address for the arduino ethernet shield.
static uint8_t ip[] = { 192, 168, 0, 1 };

// the webduino web server.
WebServer webserver("" , 80); // don't use PREFIX for root directory!

// the web root directory.
#define WWW_ROOT "/"

// web interface xhtml page changeable messages.
#define PAGE_HEADER "Arduino Embedded Web Interface Controller - V1.0"
#define PAGE_TAG "Take the control of your embedded via the WEB!"
#define PAGE_FOOTER "Copyright (C) 2010 Efstathios Chatzikyriakidis ( GNU General Public License Version 3 )"

// web interface xhtml page header content.
P(contentHeaderHtml) = "<!DOCTYPE html\n"
                       "   PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n"
                       "   \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                       "\n"
                       "<html xmlns='http://www.w3.org/1999/xhtml' xml:lang='en' lang='en'>\n"
                       " <head>\n"
                       "  <meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />\n"
                       "  <title>"PAGE_HEADER"</title>\n"
                       "  <style type='text/css'>\n"
                       "   body {\n"
                       "     margin: 0px;\n"
                       "     font: normal small Arial, Helvetica, sans-serif;\n"
                       "     background-color: #CFCFB0;\n"
                       "   }\n"
                       "\n"
                       "   .logo {\n"
                       "     text-transform: uppercase;\n"
                       "     font-size: 25px;\n"
                       "     color: #323844;\n"
                       "     font-weight: bold;\n"
                       "     text-decoration: none;\n"
                       "   }\n"
                       "\n"
                       "   input[type=text] {\n"
                       "     border: 1px solid #000000;\n"
                       "     background-color: #E2E2D2;\n"
                       "     color: #000000;\n"
                       "     padding-left: 4px;\n"
                       "     padding-right: 4px;\n"
                       "     width: 100px;\n"
                       "   }\n"
                       "\n"
                       "   .tag {\n"
                       "     margin: 0;\n"
                       "     font-size: 13px;\n"
                       "     font-weight: bold;\n"
                       "     color: #656558;\n"
                       "     letter-spacing: 2px;\n"
                       "     text-decoration: none;\n"
                       "   }\n"
                       "\n"
                       "   .link {\n"
                       "     text-transform: uppercase;\n"
                       "     font-size: 13px;\n"
                       "     color: #323844;\n"
                       "     font-weight: bold;\n"
                       "     text-decoration: none;\n"
                       "   }\n"
                       "\n"
                       "   .link:hover {\n"
                       "     text-transform: uppercase;\n"
                       "     font-size: 13px;\n"
                       "     color: #8E8E79;\n"
                       "     font-weight: bold;\n"
                       "     text-decoration: overline underline;\n"
                       "   }\n"
                       "\n"
                       "   .content {\n"
                       "     text-align: justify;\n"
                       "     line-height: 120%;\n"
                       "     padding: 10px;\n"
                       "   }\n"
                       "\n"
                       "   .heading {\n"
                       "     text-transform: uppercase;\n"
                       "     font-size: 14px;\n"
                       "     font-weight: bold;\n"
                       "     color: #81816D;\n"
                       "   }\n"
                       "\n"
                       "   .border {\n"
                       "     border: 1px solid #8E8E79;\n"
                       "   }\n"
                       "\n"
                       "   .redbox {\n"
                       "     background-color: #FF5800;\n"
                       "     border-bottom: 1px solid #F1F1D8;\n"
                       "     border-right: 1px solid #F1F1D8;\n"
                       "     border-left: 1px solid #F1F1D8;\n"
                       "   }\n"
                       "  </style>\n"
                       " </head>\n"
                       " <body>\n"
                       "  <table width='780' border='0' align='center' cellpadding='0' cellspacing='0'>\n"
                       "   <tr>\n"
                       "    <td class='redbox'>&nbsp;</td>\n"
                       "   </tr>\n"
                       "   <tr><td>&nbsp;</td></tr>\n"
                       "   <tr>\n"
                       "    <td align='center'>\n"
                       "     <table border='0' cellspacing='0' cellpadding='0'>\n"
                       "      <tr>\n"
                       "       <td align='center'><a href='/' class='logo'>"PAGE_HEADER"</a></td>\n"
                       "      </tr>\n"
                       "      <tr><td height='3'></td></tr>\n"
                       "      <tr>\n"
                       "       <td align='center'><a href='/' class='tag'>"PAGE_TAG"</a></td>\n"
                       "      </tr>\n"
                       "     </table>\n"
                       "    </td>\n"
                       "   </tr>\n"
                       "   <tr><td>&nbsp;</td></tr>\n"
                       "   <tr>\n"
                       "    <td bgcolor='#E2E2D2' class='border' height='40'>\n"
                       "     <table border='0' cellspacing='0' cellpadding='0'>\n"
                       "      <tr>\n"
                       "       <td width='10'>&nbsp;</td>\n"
                       "       <td class='heading'>Menu Options [</td>\n"
                       "       <td>\n"
                       "        <table border='0' cellspacing='0' cellpadding='5'>\n"
                       "         <tr>\n"
                       "          <td><a href='"WWW_ROOT"status' class='link'>Status</a></td>\n"
                       "          <td>|</td>\n"
                       "          <td><a href='"WWW_ROOT"pins' class='link'>Pins</a></td>\n"
                       "         </tr>\n"
                       "        </table>\n"
                       "       </td>\n"
                       "       <td class='heading'>]</td>\n"
                       "      </tr>\n"
                       "     </table>\n"
                       "    </td>\n"
                       "   </tr>\n"
                       "   <tr><td>&nbsp;</td></tr>\n"
                       "   <tr>\n"
                       "    <td bgcolor='#F3F3F3' class='border'>\n"
                       "     <table border='0' cellspacing='0' cellpadding='0'>\n"
                       "      <tr>\n"
                       "       <td class='content'>\n";

// web interface xhtml page footer content.
P(contentFooterHtml) = "       </td>\n"
                       "      </tr>\n"
                       "     </table>\n"
                       "    </td>\n"
                       "   </tr>\n"
                       "   <tr><td>&nbsp;</td></tr>\n"
                       "   <tr>\n"
                       "    <td align='center' bgcolor='#E2E2D2' class='border' height='40'>\n"
                       "     "PAGE_FOOTER"\n"
                       "    </td>\n"
                       "   </tr>\n"
                       "   <tr><td>&nbsp;</td></tr>\n"
                       "  </table>\n"
                       " </body>\n"
                       "</html>";

// root index page function handler.
void
rootPageCmd(WebServer &server, WebServer::ConnectionType type, char *urlTail, bool tailComplete) {
  // sends the standard "we're all OK" headers back to the browser.
  server.httpSuccess();

  // GET/POST: send html page, HEAD: send nothing. */
  if (type != WebServer::HEAD) {
    // store the core message of the page.
    P(rootIndexMsg) = "Welcome to the controller!";

    // send html page to the browser.
    server.printP(contentHeaderHtml);
    server.printP(rootIndexMsg);
    server.printCRLF();
    server.printP(contentFooterHtml);
  }
}

// failure page function handler.
void
failurePageCmd(WebServer &server, WebServer::ConnectionType type, char *urlTail, bool tailComplete) {
  // sends the standard "we're all OK" headers back to the browser.
  server.httpSuccess();

  // GET/POST: send html page, HEAD: send nothing. */
  if (type != WebServer::HEAD) {
    // store the core message of the page.
    P(failurePageMsg) = "Failure: Page Not Found!";

    // send html page to the browser.
    server.printP(contentHeaderHtml);
    server.printP(failurePageMsg);
    server.printCRLF();
    server.printP(contentFooterHtml);
  }
}

// output or control the status of the PINS.
void
outputPins(WebServer &server, WebServer::ConnectionType type, bool addControls = false) {
  // gui data.
  server << "<b>Digital Output Values.</b>";
  server << "<p><b>Standard Logic :</b></p>";

  // open form tag (in control mode).
  if (addControls)
    server << "<form action='"WWW_ROOT"stdpins' method='post'>";

  // looping through the digital standard logic PINS.
  for (int i = 0; i < MAX_DIGITAL_STDLOGIC_OUTS; ++i) {
    // get the PIN number and the value of it.
    int pin = DIGITAL_OUT_STDLOGIC_PINS[i].num;
    int val = DIGITAL_OUT_STDLOGIC_PINS[i].val;

    // gui data.
    server << "Pin " << pin << " : ";

    // add radio buttons (in control mode).
    if (addControls) {
      char pinName[4];
      pinName[0] = 'd';
      itoa(pin, pinName + 1, 10);
      server.radioButton(pinName, "1", "On", val);
      server << " ";
      server.radioButton(pinName, "0", "Off", !val);
    }
    else
      // just print HIGH/LOW depending on the value.
      server << (val ? "HIGH" : "LOW");

    // gui data.
    server << "<br />";
  }

  // print submit button and close form tag (in control mode).
  if (addControls)
    server << "<br /><input type='submit' value='Change States' /></form>";

  // gui data.
  server << "<p><b>PWM :</b></p>";

  // open form tag (in control mode).
  if (addControls)
    server << "<form action='"WWW_ROOT"pwmpins' method='post'>";

  // looping through the digital PWM PINS.
  for (int i = 0; i < MAX_DIGITAL_PWM_OUTS; ++i) {
    // get the PIN number and the value of it.
    int pin = DIGITAL_OUT_PWM_PINS[i].num;
    int val = DIGITAL_OUT_PWM_PINS[i].val;

    // gui data.
    server << "Pin " << pin << " : ";

    // add input textboxes (in control mode).
    if (addControls) {
      char pinName[4];
      pinName[0] = 'p';
      itoa(pin, pinName + 1, 10);
      server << "<input type='text' name='" << pinName << "' value='" << val << "' /><br />";
    }
    else
      // just print the value of the PIN.
      server << val;

    // gui data.
    server << "<br />";
  }

  // print submit button and close form tag (in control mode).
  if (addControls)
    server << "<input type='submit' value='Change Values' /></form>";

  // gui data.
  server << "<p><b>Analog Input Values.</b></p>";

  // looping through the analog input PINS.
  for (int i = 0; i < MAX_ANALOG_INPUTS; ++i) {
    // get the PIN number and the value of it.
    int pin = ANALOG_INPUT_PINS[i];
    int val = analogRead(pin);

    // gui data.
    server << "Pin " << pin << " : " << val << "<br />";
  }
}

// PINS status page function handler.
void
statusPageCmd(WebServer &server, WebServer::ConnectionType type, char *urlTail, bool tailComplete) {
  // sends the standard "we're all OK" headers back to the browser.
  server.httpSuccess();

  // GET/POST: send html page, HEAD: send nothing. */
  if (type != WebServer::HEAD) {
    // send html page to the browser.
    server.printP(contentHeaderHtml);
    outputPins(server, type, false); // output PINS with no control ability.
    server.printP(contentFooterHtml);
  }
}

// PINS control page function handler.
void
pinsPageCmd(WebServer &server, WebServer::ConnectionType type, char *urlTail, bool tailComplete) {
  // sends the standard "we're all OK" headers back to the browser.
  server.httpSuccess();

  // GET/POST: send html page, HEAD: send nothing. */
  if (type != WebServer::HEAD) {
    // send html page to the browser.
    server.printP(contentHeaderHtml);
    outputPins(server, type, true); // output PINS with control ability.
    server.printP(contentFooterHtml);
  }
}

// POST page for processing digital standard logic PINS.
void
stdPinsPageCmd(WebServer &server, WebServer::ConnectionType type, char *urlTail, bool tailComplete) {
  // handle only POST request.
  if (type == WebServer::POST) {
    bool repeat;    // are there any more POST parameters;
    char name[16];  // the name of the POST parameter.
    char value[16]; // the value of the POST parameter.

    do {
      // read the next POST parameter (get name and value).
      repeat = server.readPOSTparam(name, 16, value, 16);

      // if parameter name starts as a digital PIN.
      if (name[0] == 'd') {
        // get the PIN number.
        int pin = strtoul(name + 1, NULL, 10);

        // find the index on the array for the PIN.
        for (int i = 0; i < MAX_DIGITAL_STDLOGIC_OUTS; ++i) {
          if(DIGITAL_OUT_STDLOGIC_PINS[i].num == pin) {
            // get the value on that PIN.
            int val = strtoul(value, NULL, 10);

            // store the appropriate value on the PIN.
            DIGITAL_OUT_STDLOGIC_PINS[i].val = val ? HIGH : LOW;

            break; // stop searching.
          }
        }    
      }
    } while (repeat); // keep fetching POST parameters.

    // redirect to the base page.
    server.httpSeeOther(WWW_ROOT"pins");
  }
  else
    // say the browser that this page does not exist.
    server.httpSeeOther(WWW_ROOT"failure");
}

// POST page for processing digital PWM PINS.
void
pwmPinsPageCmd(WebServer &server, WebServer::ConnectionType type, char *urlTail, bool tailComplete) {
  // handle only POST request.
  if (type == WebServer::POST) {
    bool repeat;    // are there any more POST parameters;
    char name[16];  // the name of the POST parameter.
    char value[16]; // the value of the POST parameter.

    do {
      // read the next POST parameter (get name and value).
      repeat = server.readPOSTparam(name, 16, value, 16);

      // if parameter name starts as a digital PWM PIN.
      if (name[0] == 'p') {
        // get the PIN number.
        int pin = strtoul(name + 1, NULL, 10);

        // find the index on the array for the PIN.
        for (int i = 0; i < MAX_DIGITAL_PWM_OUTS; ++i) {
          if(DIGITAL_OUT_PWM_PINS[i].num == pin) {
            // assume that the value of the pin is LOW.
            int val = LOW;

            // if the user inputs a non-zero characters value.
            if(strlen(value) > 0) {
              // get the value on the PIN.
              val = strtoul(value, NULL, 10);
            }

            // check the limits of the value.
            val = constrain (val, 0, 255);

            // store the user value on the PIN.
            DIGITAL_OUT_PWM_PINS[i].val = val;

            break; // stop searching.
          }    
        }
      }
    } while (repeat); // keep fetching POST parameters.

    // redirect to the base page.
    server.httpSeeOther(WWW_ROOT"pins");
  }
  else
    // say the browser that this page does not exist.
    server.httpSeeOther(WWW_ROOT"failure");
}

// startup point entry (runs once).
void
setup() {
  // set digital standard logic PINS as output.
  for (int i = 0; i < MAX_DIGITAL_STDLOGIC_OUTS; ++i)
    pinMode(DIGITAL_OUT_STDLOGIC_PINS[i].num, OUTPUT);

  // set digital PWM PINS as output.
  for (int i = 0; i < MAX_DIGITAL_PWM_OUTS; ++i)
    pinMode(DIGITAL_OUT_PWM_PINS[i].num, OUTPUT);

  // initialize the ethernet device.
  Ethernet.begin(mac, ip);

  // set the default and failure pages of the web server.
  webserver.setDefaultCommand(&rootPageCmd);
  webserver.setFailureCommand(&failurePageCmd);

  // set some subpages of the web server.
  webserver.addCommand("pins", &pinsPageCmd);
  webserver.addCommand("status", &statusPageCmd);
  webserver.addCommand("failure", &failurePageCmd);
  webserver.addCommand("stdpins", &stdPinsPageCmd);
  webserver.addCommand("pwmpins", &pwmPinsPageCmd);

  // begin running the web server.
  webserver.begin();
}

// renew digital standard logic PINS' values (ignore PWM PINS).
void renewStdLogicPins (void) {
  // loop through the digital standard logic PINS.
  for (int i = 0; i < MAX_DIGITAL_STDLOGIC_OUTS; ++i) {
    // get the PIN number and the value of it.
    int pin = DIGITAL_OUT_STDLOGIC_PINS[i].num;
    int val = DIGITAL_OUT_STDLOGIC_PINS[i].val;
 
    bool found = false; // PWM PIN found.

    // loop through the digital PWM PINS.
    for (int j = 0; j < MAX_DIGITAL_PWM_OUTS; ++j) {
      // check if the PIN is also a digital PWM PIN.
      if (pin == DIGITAL_OUT_PWM_PINS[j].num) {
        found = true; // we have found that is also a PWM PIN.
        break;        // stop searching.
      }
    }

    // if the PIN is not a PWM PIN write its value.    
    if (!found)
      digitalWrite(pin, val);
  }
}

// renew PWM PINS' values (higher priority: analog values).
void renewPwmPins (void) {
  // loop through the digital PWM PINS.
  for (int i = 0; i < MAX_DIGITAL_PWM_OUTS; ++i) {
    // get the PIN number and the value of it.
    int pin = DIGITAL_OUT_PWM_PINS[i].num;
    int val = DIGITAL_OUT_PWM_PINS[i].val;

    // higher priority: analog values.
    if (val > LOW) {
      // write to the PWM PIN the analog value.
      analogWrite(pin, val);
    }
    else {
      // find the index on the array for the PIN.
      for (int j = 0; j < MAX_DIGITAL_STDLOGIC_OUTS; ++j) {
        if (pin == DIGITAL_OUT_STDLOGIC_PINS[j].num) {
          // write to the PWM PIN the digital value.
          digitalWrite(pin, DIGITAL_OUT_STDLOGIC_PINS[j].val);

          break; // stop searching.
        }
      }
    }
  }
}

// loop the main sketch.
void
loop() {
  // process any request from the web server.
  webserver.processConnection();

  // renew digital standard logic PINS' values (ignore PWM PINS).
  renewStdLogicPins();

  // renew PWM PINS' values (higher priority: analog values).
  renewPwmPins();
}
