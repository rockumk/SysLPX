# SysLPX
Specialized Version of masc4ii's Sysex Live  (https://github.com/masc4ii/SysexLive)


Adds:

   - Row selection via incoming MIDI Program Changes.
   - Number of Synths changed from 2 to 4.
   - Loads patchlist "default.syxml" by default.
   - New last row which loads row with a double-click.


Issues
  - Not sure I provided for systems with no MIDI input.
  - Most if not all new lines and/or sections marked with "rockumk" so that my poor programming could be quickly checked.
  - Couldn't figure out how to send row with a single rather than double click or how to make the cell non-editable.
  - Had to use a different version of Qmidi which inlcuded the ablitity to check if the MIDI input port was open and close it.
  - I had to add a line to Qmidi's qmidiin.cpp to deal with Program changes.
  - Probably others I can't think of.
  
  
My goal is to eventually create a specialized version for certain pieces of musical gear, but I thought these changes might benifit Sysex Live for all users. I would not trust it until masc4ii has a chance to look it over.


