# OutlinerShortcuts

**Outliner Shortcuts** is an editor only plugin that allows the binding of shortcuts to some basic *World Outliner* functions like `Collapse All` and `Expand All`, and a new function `Collapse to Root` was also added.  
Another functionality of the plugin is to automatically `Expand`/`Collapse`/`Collapse to Root` the *World Outliner* when a Map opens.  

[WIKI](https://github.com/Voulz/OutlinerShortcuts/wiki)  

## UE 5.1
Unreal Engine 5.1 added the possibility of having multiple Outliners.  
The commands `Expand All`, `Collapse All` and `Cpllapse to Root` will now work **only** on the Outliner that was last focused, refered to as the most recently used Outliner.  
Three new commands have been added: `Expand All Outliners`, `Collapse All Outliners` and `Collapse All Outliners to Root` which will affect **all** the opened Outliner windows.

# Binding Shortcuts

Open the **Editor Preferences** (`Edit > Editor Preferences`).  
Either search for **Outliner Shortcuts** or go under `General > Keyboard Shortcuts > Outliner Shortcuts`.  
You can now set custom shortcuts for the *World Outliner* functions `Collapse All`, `Collapse to Root` and `Expand All`.  
Since UE 5.1, you can also set shortcuts for the functions `Expand All Outliners`, `Collapse All Outliners` and `Collapse All Outliners to Root`.

# Automatic Collapsing on Map Open

Open the **Editor Preferences** (`Edit > Editor Preferences`).  
Either search for **Outliner Shortcuts** or go under `Plugins > Outliner Shortcuts > Outliner Shortcuts`.  
You can now set a default behavior for the *World Outliner* when a Map opens.

# Supporting my work

If you find any useful and they save you time or money, please consider supporting my work! :)  
[Buy me a coffee](https://www.buymeacoffee.com/voulz) or [Donate](https://www.paypal.com/donate/?business=8DUZ3UMZ28P9Y&no_recurring=1&item_name=I+create+plugins+for+multiple+software+like+Unreal+Engine.+If+you+find+any+useful%2C+please+consider+supporting+my+work%21+%3A%29&currency_code=AUD)

# Changelog

v1.1 (2022 11-21):
- Added support for UE 5.1 which allows multiple Outliners.  
v1.0 (2022 08-03):
- Initial Release

# Technical Information
**Features**:
- Keyboard binding of World Outliner functions: Expand All and Collapse All
- Keyboard binding of a new World Outliner functions: Collapse to Root
- Automatic Collapsing/Expanding of the Outliner on Map open.

**Code Module:**
- OutlinerShortcutsEditor [Editor]

**Number of C++ Classes:** 5  
**Supported Development Platforms:** Windows  
**Documentation:** https://github.com/Voulz/OutlinerShortcuts/  
**Important/Additional Notes:**  
This is an editor only plugin adding few simple commands for ease of working within Unreal Engine.
