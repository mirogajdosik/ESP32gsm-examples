
#if isSMTP
#include "ESP32_MailClient.h"
#define SMTPdebugLev1   1
#define SMTPdebugLev2   2

SMTPData smtpData;

//void sendCallback(SendStatus info);

//void email_send(void) {
bool email_send(const char *recipient, const char *subject, const char *message) {
  bool ret=true;
  if(SMTPdebug & SMTPdebugLev1) Serial.print(F("Sending email "));
  if(!SMTPenabled) {
    if(SMTPdebug & SMTPdebugLev1) Serial.println(F("disable !!!"));
    return false;
  }
  if(SMTPdebug & SMTPdebugLev1) {
    Serial.println(F("..."));
    Serial.print("name: ");
    Serial.println(SMTPuserName);
    Serial.print("pass: <");
    Serial.print(SMTPpassword);
    Serial.println(">");
  }
  if(SMTPdebug & SMTPdebugLev2) smtpData.setDebug ( true );
  else smtpData.setDebug ( false );
  //Set the Email host, port, account and password
  //  smtpData.setLogin("outlook.office365.com", 587, "YOUR_EMAIL_ACCOUNT@outlook.com", "YOUR_EMAIL_PASSWORD");
  smtpData.setLogin(SMTPserverName, SMTPserverPort, SMTPuserName, SMTPpassword);



  
  //For library version 1.2.0 and later which STARTTLS protocol was supported,the STARTTLS will be 
  //enabled automatically when port 587 was used, or enable it manually using setSTARTTLS function.
  //smtpData.setSTARTTLS(true);

  //Set the sender name and Email
  smtpData.setSender(SMTPsenderName, SMTPuserName);

  //Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
  smtpData.setPriority("High");

  //Set the subject
  smtpData.setSubject(subject);

  //Set the message - normal text or html format
  //smtpData.setMessage("<div style=\"color:#ff0000;font-size:20px;\">Hello World! - From ESP32</div>", true);
  smtpData.setMessage(message,false);
  //Add recipients, can add more than one recipient
  //smtpData.addRecipient("SOME_RECIPIENT@SOME_MAIL.com");
  smtpData.addRecipient(recipient);

  
  smtpData.setSendCallback(sendCallback);

  //Start sending Email, can be set callback function to track the status
  if (!MailClient.sendMail(smtpData)) {
    ret=false;
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
  } else {
    ASTATE.mail_send_cnt_total++;
    ASTATE.mail_send_cnt_day++;
  }
  //Clear all data from Email object to free memory
  smtpData.empty();
  return ret;
}


//Callback function to get the Email sending status
void sendCallback(SendStatus msg)
{
  //Print the current status
  if(SMTPdebug & SMTPdebugLev1) Serial.println(msg.info());

  //Do something when complete
  if (msg.success())
  {
    if(SMTPdebug & SMTPdebugLev1) Serial.println("----------------");
  }
}
#endif //isSMTP
