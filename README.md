# SGDK Skeleton Project

This project is based on the YouTube tutorial series by Pigsy's Retro Game Dev Tutorials, focusing on Sega Genesis (Mega Drive) development using the SGDK (Sega Genesis Development Kit).

## Tutorial Reference
- **YouTube Series:** [Pigsy's Retro Game Dev Tutorials](https://www.youtube.com/playlist?list=PL8h4jt35t1wlT4lBzQKz5lQKQnQ1g0QWk)  
  (Search for "Pigsy SGDK tutorial" if the link changes)
- **SGDK:** https://github.com/Stephane-D/SGDK

## Project Structure
- `src/` — Source code (main game logic in `main.c`)
- `res/` — Resources (images, palettes, etc.)
- `Makefile` — Build instructions

## Getting Started
1. **Install SGDK**
   - Download and set up SGDK as described in the tutorial or from the [official repo](https://github.com/Stephane-D/SGDK).
   - Set the `GDK` environment variable to point to your SGDK directory.

2. **Clone or Download this Project**
   - Place it in your SGDK `projects` folder or any directory you prefer.

3. **Build the ROM**
   - Open a terminal in the project directory.
   - Run:
     ```sh
     make
     ```
   - The output ROM will be in the `out/` directory (or as specified in your Makefile).

4. **Run in Emulator**
   - Use your favorite Sega Genesis emulator to test the generated ROM.

## What This Project Does
- Loads background and foreground images.
- Demonstrates palette usage and basic scrolling.
- Closely follows the code and concepts from Pigsy's tutorial series.

## Customization
- Replace images in `res/` and update `resources.h` as needed.
- Modify `src/main.c` to experiment with scrolling, palettes, and more.

## Credits
- Pigsy's Retro Game Dev Tutorials for the excellent beginner series.
- Stephane Dallongeville for SGDK.

---
Happy coding and enjoy Sega Genesis development!
