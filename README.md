# sigfoxpropbomb
This is an attempt to make my arduino-propbomb (see other repo's) traceable by using the SigFox network. Later these projects will be merged

# Hardware used
- Arduino MKRFOX1200
- Grove LCD RGB Backlight
- Pushbutton

# Usage
When armed (by pushbutton) a message with countdown time will be sent to the backend. By pressing the button again the timer will start running and send a message to the backend every minute with time left and status. The display shows the time and serial interface is used for debugging/monitoring messages

# Tracking
The SigFox backend sends its data to AWS, where it triggers the storage of data in S3. On Freeboard this data is read and displayed on this link: https://freeboard.io/board/5a154977511f114a61001f8e

# Future features
By merging this with the original board it will be possible to set the timer, passcode and which wire to cut for disarm. There will also be an option to set message interval and the screen won't freeze when sending the message
