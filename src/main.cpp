#include "ncurses_display.h"
#include "system.h"

int main() {
  System system;
  //
   //setenv("TERM", "xterm-256color", 0);
   //std::string s = getenv("TERM");
  NCursesDisplay::Display(system);
}