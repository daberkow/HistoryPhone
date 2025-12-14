import * as React from "react"
import "./mainpage.css"
import { API_ENDPOINT, Data, Folder } from "../types"

interface MainPageProps {
    selectedEvent: Folder | null;
}

export default function MainPage({ selectedEvent }: MainPageProps) {
    const handlePhotoChange = async (event: React.MouseEvent<HTMLInputElement>) => {
        // When a photo is clicked ,we need to put it up in the main window
        const photoUrl = selectedEvent ? `./content/${selectedEvent.year}/${event.currentTarget.id}.jpg` : "./gallery.png";
        console.log("Photo clicked:", photoUrl);
        const playWindow = document.getElementById("playWindow");
        if (playWindow) {
            playWindow.innerHTML = `<img src="${photoUrl}" alt="Selected Photo" />`;
        }
    }

    const handleAudioClicked = async (event: React.MouseEvent<HTMLInputElement>) => {
        const id = event.currentTarget.id;
        const res = await fetch('/api/onhook')
        const onHook = await res.text();
        // 0 is the Phone is picked up
        // 1 is on the hook
        if (onHook === "1") {
            // Queue the audio to play when the phone is picked up
            const params = "queue=" + id;
            try {
                const response = await fetch('/api/queue', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/x-www-form-urlencoded',
                    },
                    body: params,
                });

                if (!response.ok) {
                    console.error('Failed to update queue on the server');
                } else {
                    alert("Please pick up the phone to play audio.");
                }
            } catch (error) {
                console.error('Error updating queue:', error);
            }
            return;
        }
    }

    const handleTextClicked = async (event: React.MouseEvent<HTMLDivElement>) => {
        // When a text file is clicked, display it in the main window
        const fileIndex = event.currentTarget.id.split('_')[2];
        const textUrl = selectedEvent ? `./content/${selectedEvent.year}/${fileIndex}.txt` : "";
        console.log("Text file clicked:", textUrl);

        try {
            const response = await fetch(textUrl);
            const textContent = await response.text();
            const playWindow = document.getElementById("playWindow");
            if (playWindow) {
                playWindow.innerHTML = `<div style="padding: 20px; text-align: left; white-space: pre-wrap;">${textContent}</div>`;
            }
        } catch (error) {
            console.error('Error loading text file:', error);
            const playWindow = document.getElementById("playWindow");
            if (playWindow) {
                playWindow.innerHTML = `<div style="padding: 20px;">Error loading text file</div>`;
            }
        }
    }

    return (
        <div id="mainpage">
            <div id="playWindow">
                {selectedEvent ? (
                    <div>
                        <h1>{selectedEvent.year}</h1>
                    </div>
                ) : (
                    <h2>Select a year from the sidebar</h2>
                )}
            </div>
            <div id="thumbnails">
                {selectedEvent ? (
                    <div>
                        {Array.from({ length: selectedEvent.mp3_files }).map((_, i) => (
                            <div className="mp3Tile" key={i} id={"mp3_" + selectedEvent.year + "_" + i} onClick={handleAudioClicked}>
                                <img src="./music-file.png"></img><p>MP3 File {i + 1}</p>
                            </div>
                        ))}
                        {Array.from({ length: selectedEvent.photos }).map((_, i) => (
                            <div className="photoTile" key={i} id={1 + i + ""} onClick={handlePhotoChange}>
                                <img src={`./content/${selectedEvent.year}/${i + 1}.jpg`}></img>
                                <p>Photo {i + 1}</p>
                            </div>
                        ))}
                        {Array.from({ length: selectedEvent.text_documents }).map((_, i) => (
                            <div className="textTile" key={i} id={"text_" + selectedEvent.year + "_" + (i + 1)} onClick={handleTextClicked}><img src="./file.png"></img><p>Text Document {i + 1}</p></div>
                        ))}
                    </div>
                ) : ("")}
            </div>
        </div>
    );
}