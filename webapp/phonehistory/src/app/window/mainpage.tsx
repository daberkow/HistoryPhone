import * as React from "react"
import "./mainpage.css"
import { Data, Folder } from "../types"

interface MainPageProps {
    selectedEvent: Folder | null;
}

export default function MainPage({ selectedEvent }: MainPageProps) {
    return (
        <div id="mainpage">
            <div id="playWindow">
                {/* <h2>Actual Window</h2> */}
                {selectedEvent ? (
                    <div>
                        <h2>{selectedEvent.year}</h2>
                        <p>MP3s: {selectedEvent.mp3_files}</p>
                        <p>Photos: {selectedEvent.photos}</p>
                        <p>Text Documents: {selectedEvent.text_documents}</p>
                    </div>
                ) : (
                    <h2>Please select an event from the sidebar</h2>
                )}
            </div>
            <div id="thumbnails">
                {/* <h2>Gallery</h2> */}
            </div>
        </div>
    );
}