# HistoryPhone

The idea of this project is to take an old rotary phone, add an esp32, and when
someone dials a year, to get a story from the family for that year. In addition
to the story, there can be photos along with it that are displayed via a web app
on a third party devices such as an iPad.

## Usage

Join a wifi device to the network "HistoryPhone", then browse to "phone.local".
Then proceed to pick up the handset and dial a year. Returning the handset
will end the content. If no content is selected, after a few seconds it will
give a busy signal.

## SD Card Folder Layout

In the root of the SD card there are two folders, "content" and "web". Web hosts
the built version fo the Next.JS web frontend. The content folder contains a few
audio clips, such as "tone.mp3" which gives a ring tone. These tones are from
"pixabay.com" and are for use as long as not sold. Also within content are
sub-folders which are named years. Each year we want to have on the phone will
need a folder. Within those folders there can be any number of MP3s, JPGs, and
other documents. The MP3s should be named #.mp3 and will be randomly selected
from when a year is selected. You can have a corresponding #.txt document with
the following metadata:

```
speaker:
year:
year_recorded:
people_mentioned:
rating:
```

## Building

### Controller


### React App

## Wiring

## TODO:

So much...
- Photos overlayed on AI art?
- Family tree generator?
- Someway to record on device?
- Buttons and lights if phone has them?
- More of a standard for metadata

## Attribution

Icons from:
<a href="https://www.flaticon.com/free-icons/mp3" title="mp3 icons">Mp3 icons created by Freepik - Flaticon</a>
<a href="https://www.flaticon.com/free-icons/art" title="art icons">Art icons created by Kiranshastry - Flaticon</a>
<a href="https://www.flaticon.com/free-icons/document" title="document icons">Document icons created by Icongeek26 - Flaticon</a>