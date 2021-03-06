/*----------------------------buttons----------------------------*/

void setupButton() {
  //called from setup
  
  //..do setup for each end button
  for (int i = 0; i < _endButtonTotal; i++) {
    pinMode(_endButtonPin[i], INPUT_PULLUP);  //activate internal pullup resistor on pin //could do this also thru button[i].attach(pin, mode)
    _endButton[i].attach(_endButtonPin[i]);   //attach pin to empty bounce button
    _endButton[i].interval(_endButtonDebounceTime);           //unsigned long (5ms)
  }

  //do setup for each knob button
  for (int i = 0; i < _knobTotal; i++) {
    pinMode(_knobButtonPin[i], INPUT_PULLUP); //activate internal pullup resistor on pin
    _knobButton[i].attach(_knobButtonPin[i]); //attach pin to empty bounce button
    _knobButton[i].interval(_knobButtonDebounceTime);          //unsigned long (5ms)
  }

  //setup other button
  pinMode(_otherButtonPin, INPUT_PULLUP);     //activate internal pullup resistor on pin
  _otherButton.attach(_otherButtonPin);       //attach pin to empty bounce button
  _otherButton.interval(_otherButtonDebounceTime);              //unsigned long (5ms)

  //setupEndButtonInterrupts(); //OVERKILL
}

/*-----------buttons - get------------*/
void endButtonGet() {
  //sub-loop. reads all the end buttons (called from somewhere in main loop)
  for (int i = 0; i < _endButtonTotal; i++) {
    boolean hasChanged = _endButton[i].update();  //Bounce buttons keep internal track of whether any change has occured since last time
    
    if (hasChanged) {
      
      if (_endButtonBehaviour[i] == 0) {
        //momentary
        
        if (_endButton[i].fell()) {
          if (_endButtonType[i]== 0) {
            //send MIDI note On
            usbMIDI.sendNoteOn(_endButtonMidiNote[i], _endButtonMidiOnValue[i], _midiChannel); //note, val (127), midi channel
            //usbMIDI.send_now();
            //set flag for messaget to show
          } else if (_endButtonType[i] == 1) {
            //send MIDI CC  - just to send CC max on fall
            usbMIDI.sendControlChange(_endButtonMidiCC[i], _endButtonMidiOnValue[i], _midiChannel);  //control, value, channel
            //usbMIDI.send_now();
            //flashMessage
          }
        } //END .fell
        else if (_endButton[i].rose()) { 
          if (_endButtonType[i] == 0) {
            //send MIDI note Off
            usbMIDI.sendNoteOff(_endButtonMidiNote[i], _endButtonMidiOffValue[i], _midiChannel); //note, val (0), midi channel - or would better vals be either 0/64 or 65/127, smaller larger comparisons or somit like that?
            //usbMIDI.send_now();
            //flashMessage
          }
        } //END .rose
        
      } //END momentary
      else if (_endButtonBehaviour[i] == 1) {
        //toggle
        
        //need for boolean to track on/off state and/or taking into account external input/bounce-back from the sequencer
        if (_endButton[i].fell()) {
          if (_endButtonToggleState[i]) {
            //send Off
            if (_endButtonType[i] == 0) {
              //send MIDI note Off
              usbMIDI.sendNoteOff(_endButtonMidiNote[i], _endButtonMidiOffValue[i], _midiChannel); //note, val (0), midi channel - or would better vals be either 0/64 or 65/127, smaller larger comparisons or somit like that?
              //usbMIDI.send_now();
              //flashMessage
            }
          } else {
            //send On
            if (_endButtonType[i]== 0) {
              //send MIDI note On
              usbMIDI.sendNoteOn(_endButtonMidiNote[i], _endButtonMidiOnValue[i], _midiChannel); //note, val (127), midi channel
              //usbMIDI.send_now();
              //set flag for messaget to show
            } else if (_endButtonType[i] == 1) {
              //send MIDI CC  - just to send CC max on fall
              usbMIDI.sendControlChange(_endButtonMidiCC[i], _endButtonMidiOnValue[i], _midiChannel);  //control, value, channel
              //usbMIDI.send_now();
              //flashMessage
            }
          } //END send On
        } //END .fell
        else if (_endButton[i].rose()) { 
          //
        } //END .rose
        
        _endButtonToggleState[i] = !_endButtonToggleState[i]; //flip state for next time  //can be adjusted elswhere dependant on metering source
      } //END toggle
      
    } //END hasChanged

    //time tracking? ..need to track long hold on buttons
    //could just change a flag on fall and rise, 
    //but unless using iterrupts cannot garuntee that it won't 
    //miss one of them and get stuck in the wrong position
    
/*    unsigned long buttonPressTimeStamp;
    buttonPressTimeStamp = millis();
    if  ( buttonState == 1 ) {
      if ( millis() - buttonPressTimeStamp >= 500 ) {
         buttonPressTimeStamp = millis();
         if ( ledState == HIGH ) ledState = LOW;
         else if ( ledState == LOW ) ledState = HIGH;
         digitalWrite(LED_PIN, ledState );
        Serial.println("Retriggering button");
      }
    }
*/    
    if (_endButtonBehaviour[i] == 0) {
        //if momentary..
    }
    else if (_endButtonBehaviour[i] == 1) {
        //if toggle..
        //_endButtonToggleState[i]
    }
  
  } //END main for loop
} //END endButtonGet

void knobButtonGet() {
  //sub-loop. reads all the knob buttons (if used)
  for (int i = 0; i < _knobTotal; i++) {
    //NOT IN USE YET ..will need to scan cos not enough pins
    //tell each knob to change its speed OR send MIDI CC
    
    if (_knobButtonType[i] == 0) {
          //internal switch precision/speed - default
          _knobButtonBehaviour[i] = 1; //_knobButtonBehaviour is always toggle (1) in this type mode - set here just to make sure - this should be under buttons really
          //
          if (_knobButtonToggleState[i]) {
            //set _knobSpeed[2][i] to orig speed
            //set _knobSpeedAltInUse to false
            _knob[i].setCurSpeed(_knobSpeed[0][i]);
            _knobSpeedAltInUse[i] = false;
          } else {
            //set _knobSpeed[2][i] to alt speed
            //set _knobSpeedAltInUse to true
            _knob[i].setCurSpeed(_knobSpeed[1][i]);
            _knobSpeedAltInUse[i] = true;
          }
          //
        } else if (_knobButtonType[i] == 1) {
          //send MIDI CC
          if (_knobButtonBehaviour[i] == 0) {
            //momentary
            //
          } else if (_knobButtonBehaviour[i] == 1) {
            //toggle
            //
          }
        } //END send MIDI CC
        
  } //END main for loop
} //END knobButtonGet

void otherButtonGet() {
  //sub-loop. reads any other buttons
  //check we are in menu mode first
  if (_menuMode) {
    boolean hasChanged = _otherButton.update();  //Bounce buttons keep internal track of whether any change has occured since last time
    if (hasChanged) {
//      int value = _otherButton.read();
//      if (value == HIGH) {
//        //
//      } else {
//        //
//      }
      //OR
      if (_otherButton.read()) {
        //
      }
    }
  }
}

/*    ----------------OVERKILL-----------------------------
//attachInterrupt(interruptPin, function, CHANGE);
void setupEndButtonInterrupts() {
  attachInterrupt(_endButton[0], interruptEB0, CHANGE);  //FALLING, RISING, CHANGE
  attachInterrupt(_endButton[1], interruptEB1, CHANGE);
  attachInterrupt(_endButton[2], interruptEB2, CHANGE);
  attachInterrupt(_endButton[3], interruptEB3, CHANGE);
  attachInterrupt(_endButton[4], interruptEB4, CHANGE);
}

void interruptEB0() { _endButton[i].update(); }
void interruptEB1() {  }
void interruptEB2() {  }
void interruptEB3() {  }
void interruptEB4() {  }
*/
