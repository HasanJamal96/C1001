#include "Buttons.h"

void Button::begin(){
  pinMode(m_pin, INPUT);
  m_state = digitalRead(m_pin);
  m_time = millis();
  m_lastState = m_state;
  m_changed = false;
  m_lastChange = m_time;
}

bool Button::read(){
  uint32_t ms = millis();
  bool pinVal = digitalRead(m_pin);
  if (ms - m_lastChange < m_dbTime){
      m_changed = false;
  }
  else{
      m_lastState = m_state;
      m_state = pinVal;
      m_changed = (m_state != m_lastState);
      if (m_changed) m_lastChange = ms;
  }
  m_time = ms;
  return m_state;
}

bool Button::isPressed(){
    return m_state;
}

bool Button::isReleased(){
    return !m_state;
}

bool Button::wasPressed(){
    return m_state && m_changed;
}

bool Button::wasReleased(){
    return !m_state && m_changed;
}

bool Button::pressedFor(uint32_t ms){
    return m_state && m_time - m_lastChange >= ms;
}
