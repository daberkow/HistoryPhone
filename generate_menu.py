#!/usr/bin/env python3
"""
Generate a phone menu audio file listing all available year folders.
Uses gTTS (Google Text-to-Speech) to create the audio.
"""

import os
from pathlib import Path
from gtts import gTTS

def get_year_folders():
    """Get all year folders (directories with numeric names)."""
    current_dir = Path(__file__).parent
    year_folders = []

    for item in current_dir.iterdir():
        if item.is_dir() and item.name.isdigit():
            year_folders.append(item.name)

    return sorted(year_folders)

def format_year_spoken(year):
    """Format year to be spoken naturally (e.g., 1921 -> '19, 21')."""
    if len(year) == 4:
        return f"{year[:2]} {year[2:]}"
    return year

def generate_menu_text(years):
    """Generate the menu text."""
    intro = "Welcome to History Phone. Numbers available are: "
    spoken_years = [format_year_spoken(year) for year in years]
    year_list = ". ".join(spoken_years)
    return intro + year_list

def create_menu_audio(output_file="menu.mp3"):
    """Create the phone menu audio file."""
    years = get_year_folders()

    if not years:
        print("No year folders found!")
        return

    menu_text = generate_menu_text(years)
    print(f"Generating audio: {menu_text}")

    # Create TTS object and save to file
    tts = gTTS(text=menu_text, lang='en', slow=False)
    tts.save(output_file)

    print(f"Menu audio saved to: {output_file}")
    print(f"Found {len(years)} years: {', '.join(years)}")

if __name__ == "__main__":
    create_menu_audio()
