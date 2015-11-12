import processing.sound.*;

AudioDevice device;
SoundFile[] file;

int numsounds = 5;


void setup(){
  size(640, 480);
  
  device = new AudioDevice(this, 48000, 32);
  
  file = new SoundFile[numsounds];
  
  for (int i = 0; i < numsounds; i++){
    file[i] = new SoundFile(this, (i+1) + ".aif");
  }
  
  ConfigurationBuilder cb = new ConfigurationBuilder();
  cb.setOAuthConsumerKey("");
  cb.setOAuthConsumerSecret("");
  cb.setOAuthAccessToken("");
  cb.setOAuthAccessTokenSecret("");
  
  StatusListener listener = new StatusAdapter(){
    public void onStatus(Status status) {
        System.out.println(status.getUser().getName() + " : " + status.getText());
        file[(int)random(0,5)].play(.5, 1.0);
        bgcolor = color(random(0,255), random(0,255), random(0,255));
    }
    public void onException(Exception ex) {
        ex.printStackTrace();
    }
  };

  TwitterStream twitterStream = new TwitterStreamFactory(cb.build()).getInstance();
  twitterStream.addListener(listener);
  
  twitterStream.filter("Chicago");
}
color bgcolor = #000000;

void draw(){
  background(bgcolor);
}