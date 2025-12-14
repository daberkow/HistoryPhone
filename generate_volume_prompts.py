#!/usr/bin/env python3
"""
Generate volume prompt audio files for the History Phone project.
Uses Google Text-to-Speech (gTTS) to create MP3 files.

Requirements:
    pip install gtts

Usage:
    python generate_volume_prompts.py
"""

from gtts import gTTS
import os

# Create output directories if they don't exist
output_dir = "sdcard/content/volume"
os.makedirs(output_dir, exist_ok=True)

print("Generating volume prompt audio files...")

# Generate the main prompt
print("Creating volume_prompt.mp3...")
prompt_text = "Please select a volume, 1 to 21"
tts = gTTS(text=prompt_text, lang='en', slow=False)
tts.save("sdcard/content/volume_prompt.mp3")
print("  ✓ volume_prompt.mp3")

# Generate confirmation messages for volumes 1-21
print("\nCreating volume confirmation files (1-21)...")
for volume in range(1, 22):
    filename = f"{output_dir}/{volume}.mp3"
    text = f"Volume set to {volume}"

    tts = gTTS(text=text, lang='en', slow=False)
    tts.save(filename)
    print(f"  ✓ {volume}.mp3")

print("\n" + "="*50)
print("✓ All volume prompt files generated successfully!")
print("="*50)
print(f"\nFiles created in:")
print(f"  - sdcard/content/volume_prompt.mp3")
print(f"  - {output_dir}/1.mp3 through {output_dir}/21.mp3")
print("\nTotal files: 22")
