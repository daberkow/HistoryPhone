import * as React from "react"
import './sidebar.css'
import Link from 'next/link'
import { Data, Folder } from "../types"

interface SidebarProps {
    passedEvents: Data;
    onEventSelect: (event: Folder) => void;
}

export default function Sidebar({ passedEvents, onEventSelect }: SidebarProps) {
    const [volume, setVolume] = React.useState(8); // Initial volume value

    const handleVolumeChange = async (event: React.ChangeEvent<HTMLInputElement>) => {
        const newVolume = parseInt(event.target.value, 10);
        setVolume(newVolume);

        const params = "volume=" + newVolume;
        try {
            // Make an AJAX call to update the volume on the server
            const response = await fetch('/api/volume', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: params,
            });

            if (!response.ok) {
                console.error('Failed to update volume on the server');
            }
        } catch (error) {
            console.error('Error updating volume:', error);
        }
    };

    return (
        <div className='sidebar'>
            <h4>Years</h4>
            <hr></hr>
            {
                passedEvents.folders.map((singleEvent, index) => (
                    <div
                        className='sidebar-item'
                        key={singleEvent.year || index}
                        onClick={() => onEventSelect(singleEvent)}>
                        <h3>{singleEvent.year}</h3>
                    </div>
                ))
            }
            <div style={{ position: 'absolute', bottom: '8px', display: 'flex', justifyContent: 'center', width: '100%'}}>
                {/* Volume controls */}
                <h3 style={{ paddingRight: '12px' }}>Volume</h3>
                <input type="range" min="0" max="21" value={volume} onChange={handleVolumeChange}/>
            </div>
        </div>
    )
}
