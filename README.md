# HistoryPhone

An interactive family history experience that transforms a vintage rotary phone into a storytelling device. Dial a year, and the phone plays recorded family stories from that time period. Photos and documents are simultaneously displayed on a companion web interface accessed via tablet or smartphone. When a year is dialed, the code looks +/- 10 years to see if a story exists in that date range, if not plays a busy signal. There are a few special dial codes: 0=Operator menu, 8=Volume, 9=Random file.

## Features

- **Rotary Phone Interface**: Dial any configured year to hear family stories
- **Audio Playback**: Randomly selected MP3 recordings with metadata support
- **Visual Companion**: Web app displays photos and documents synchronized with audio
- **WiFi Access Point**: Creates its own network for easy connection
- **SD Card Storage**: Simple file-based content management
- **Volume Control**: Dial to adjust playback volume (1-21)
- **Auto-generated Menus**: TTS-generated audio prompts for available years

## Hardware Requirements

- **ESP32 Development Board**: ESP32 CAM Board or similar (4MB Flash minimum, 8MB PSRAM recommended)
  - Tested with: [ESP32 CAM Board](https://www.amazon.com/dp/B0CJJHXD1W), and later a ESP32-Audio-Kit Audio Development Board (https://www.amazon.com/dp/B0B63KZ6C1), the ESP32-Audio-Kit works well but has terrible docs, https://github.com/trombik/esp-adf-component-ai-thinker-esp32-a1s helps
  - Dual-core 32-bit microprocessor up to 240 MHz
  - WiFi and Bluetooth 4.2 (LE) support
- **Rotary Phone**: Vintage rotary phone with working dial mechanism
- **SD Card**: For storing audio files, photos, and web interface
- **I2S Audio Decoder/Amplifier**: For audio playback through the phone's speaker
- **Miscellaneous**: Wiring, resistors, and components for connecting phone to ESP32

## Software Prerequisites

### For Controller Development

- **PlatformIO**: Install via VS Code extension or CLI
- **Platform**: ESP-IDF 5.5.1 with Arduino 3.3.4 (automatically installed via platformio.ini)
- **Python 3**: Required for build tools

### For Audio Generation Scripts

Both `generate_menu.py` and `generate_volume_prompts.py` require:

```bash
pip install gtts
```

### For Web App Development

- **Node.js**: Version 20 or higher
- **npm**: Comes with Node.js

## Quick Start Guide

1. **Prepare SD Card**:
   - Format SD card (FAT32 recommended)
   - Create folder structure: `content/` and `web/`
   - Add year folders (e.g., `content/1945/`, `content/1967/`)
   - Place MP3 files in year folders (named `1.mp3`, `2.mp3`, etc.)
   - Add corresponding images and metadata files

2. **Generate Audio Prompts**:
   ```bash
   pip install gtts
   python generate_menu.py
   python generate_volume_prompts.py
   ```

3. **Build Web App**:
   ```bash
   cd webapp
   npm install
   npm run build
   # Copy webapp/out/* to SD card's web/ folder
   ```

4. **Build and Flash Controller**:
   ```bash
   cd controller
   pio run --target upload
   ```

5. **Insert SD Card** into ESP32 and power on

6. **Connect to WiFi**:
   - Network: "HistoryPhone"
   - URL: http://phone.local

## Usage

### Connecting to the Phone

1. **Connect to WiFi**:
   - On your tablet/smartphone, connect to the WiFi network "HistoryPhone"
   - Open a web browser and navigate to `http://phone.local`

2. **Using the Phone**:
   - Pick up the handset
   - Listen to the menu of available years
   - Dial a 4-digit year (e.g., 1945)
   - The phone will play a random story from that year
   - Photos and documents will display on the web interface
   - Hang up the handset to stop playback

3. **Volume Control**:
   - Dial special volume codes to adjust playback volume
   - Volume levels range from 1 to 21

4. **Timeout**:
   - If no year is dialed within a few seconds, the phone will give a busy signal

## SD Card Folder Layout

The SD card should be formatted with the following structure:

```
SD_CARD/
├── web/                    # Web application files (copy from webapp/out/)
│   ├── index.html
│   ├── _next/
│   └── ...
├── content/
│   ├── menu.mp3           # Auto-generated menu of available years
│   ├── tone.mp3           # Ring tone (from pixabay.com)
│   ├── volume_prompt.mp3  # Volume selection prompt
│   ├── volume/
│   │   ├── 1.mp3         # Volume confirmation messages
│   │   ├── 2.mp3
│   │   └── ... (through 21.mp3)
│   ├── 1945/              # Year folder (example)
│   │   ├── 1.mp3         # Story recording
│   │   ├── 1.txt         # Metadata for story 1
│   │   ├── 1.jpg         # Photo associated with story 1
│   │   ├── 2.mp3         # Another story from 1945
│   │   ├── 2.txt
│   │   └── family_photo.jpg
│   ├── 1967/              # Another year folder
│   │   ├── 1.mp3
│   │   └── 1.txt
│   └── ...
```

### Content Guidelines

- **Web folder**: Contains the built Next.js static site. Copy contents from `webapp/out/` after building.
- **Audio files**: Ring tones and system sounds from [Pixabay](https://pixabay.com) (free for non-commercial use).
- **Year folders**: Create a folder for each year you want available (must be numeric, e.g., `1945`).
- **Story files**: Name MP3 files numerically (`1.mp3`, `2.mp3`, etc.). A random file is selected when that year is dialed.
- **Images**: JPG files in year folders are displayed on the web interface.
- **Metadata**: Optional `.txt` files with the same number as the MP3 provide additional information.

### Metadata Format

Each story can have a corresponding `.txt` file (e.g., `1.txt` for `1.mp3`) with metadata:

```
speaker: John Smith
year: 1945
year_recorded: 2024
people_mentioned: Mary, Robert, Susan
```

## Building

### Controller (ESP32 Firmware)

The controller code is located in the `controller/` directory and uses PlatformIO for building.

1. **Install PlatformIO**:
   - Via VS Code: Install the PlatformIO IDE extension
   - Via CLI: `pip install platformio`

2. **Open Project**:
   ```bash
   cd controller
   ```

3. **Select Environment**:
   - Edit `platformio.ini` to uncomment your target board (ESP32, ESP32-S3, or ESP32-P4)
   - Default is ESP32 with 4MB Flash

4. **Build Firmware**:
   ```bash
   pio run
   ```

5. **Upload to Board**:
   ```bash
   pio run --target upload
   ```

6. **Monitor Serial Output** (optional):
   ```bash
   pio device monitor
   ```

### Web App

The web application is a Next.js static site located in the `webapp/` directory. The built output must be copied to the SD card.

1. **Install Dependencies**:
   ```bash
   cd webapp
   npm install
   ```

2. **Build for Production**:
   ```bash
   npm run build
   ```

   This creates a static export in the `out/` directory.

3. **Deploy to SD Card**:
   - Copy the contents of `webapp/out/` to the SD card at `web/`
   - The ESP32 will serve these files from the SD card

### Audio Generation Scripts

These scripts generate TTS audio files for the phone menu and volume prompts.

1. **Install gTTS**:
   ```bash
   pip install gtts
   ```

2. **Generate Menu Audio**:
   ```bash
   python generate_menu.py
   ```
   - Scans for year folders in the current directory
   - Creates `menu.mp3` listing all available years
   - Copy the generated file to `sdcard/content/menu.mp3`

3. **Generate Volume Prompts**:
   ```bash
   python generate_volume_prompts.py
   ```
   - Creates `volume_prompt.mp3` and volume confirmation files (1-21)
   - Outputs directly to `sdcard/content/volume/` and `sdcard/content/volume_prompt.mp3`

## Project Structure

```
PhoneHistory/
├── controller/           # ESP32 firmware (PlatformIO project)
│   ├── src/             # Main source code
│   ├── platformio.ini   # Build configuration
│   └── boards/          # Custom board definitions
├── webapp/              # Next.js web application
│   ├── src/             # React components and pages
│   ├── public/          # Static assets
│   ├── out/             # Built static site (after npm run build)
│   └── package.json     # Node.js dependencies
├── sdcard/              # SD card content structure
│   ├── content/         # Audio files and year folders
│   └── web/             # Deployed web app (copy from webapp/out/)
├── generate_menu.py     # Script to generate year menu audio
└── generate_volume_prompts.py  # Script to generate volume prompt audio
```

## Wiring

*TODO: Add detailed wiring diagram*

Basic connections needed:
- **Rotary Dial**: Connect to GPIO pins for pulse counting
- **Hook Switch**: Connect to GPIO for handset detection
- **I2S Audio**: Connect I2S DAC/amplifier for audio output
  - I2S BCLK, LRCLK, and DIN pins
- **SD Card**: SPI connection for SD card reader
  - MISO, MOSI, SCK, and CS pins
- **Power**: 5V power supply for ESP32 and audio amplifier

## Troubleshooting

### Common Issues

**Phone not creating WiFi network:**
- Check ESP32 power supply (needs stable 5V)
- Verify firmware uploaded successfully
- Check serial monitor for error messages

**Web interface not loading:**
- Ensure `webapp/out/` contents are copied to SD card's `web/` folder
- Verify SD card is properly inserted and formatted (FAT32)
- Try accessing by IP address instead of phone.local

**No audio playback:**
- Check I2S connections to audio decoder/amplifier
- Verify SD card has audio files in correct locations
- Check volume level (may be set too low)

**Year not recognized:**
- Ensure year folder exists in `content/` directory
- Year folders must be numeric (e.g., `1945`, not `year1945`)
- Check that folder contains at least one `.mp3` file

## Development

### Web App Development

For local development of the web interface:

```bash
cd webapp
npm install
npm run dev
```

The development server runs at `http://localhost:3000`.

### Controller Development

Monitor serial output while developing:

```bash
cd controller
pio device monitor
```

Set debug level in `platformio.ini`:
```ini
-D CORE_DEBUG_LEVEL=5  ; 0=None, 1=Error, 2=Warn, 3=Info, 4=Debug, 5=Verbose
```

## Future Ideas

- Photos overlaid on AI-generated period artwork
- Family tree generator integration
- On-device recording capability
- Support for phone buttons and indicator lights
- Standardized metadata schema
- Multi-language support
- Bluetooth audio output option

## Attribution

### Icons
- [Mp3 icons](https://www.flaticon.com/free-icons/mp3) created by Freepik - Flaticon
- [Art icons](https://www.flaticon.com/free-icons/art) created by Kiranshastry - Flaticon
- [Document icons](https://www.flaticon.com/free-icons/document) created by Icongeek26 - Flaticon

### Audio Assets
- Ring tones and system sounds from [Pixabay](https://pixabay.com) (free for non-commercial use)

### Technologies
- **ESP32-audioI2S** - Audio playback library
- **ESPAsyncWebServer** - Web server for ESP32
- **Next.js** - React framework for web interface
- **gTTS** - Google Text-to-Speech for audio generation
- **PlatformIO** - ESP32 development platform

## License

This project is provided as-is for personal and educational use. Audio files from Pixabay must not be resold per their license terms.

---

**HistoryPhone** - Preserving family memories, one dial at a time.
