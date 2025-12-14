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
            <div className='sidebar-items-container'>
                {
                    passedEvents.folders.map((singleEvent, index) => (
                        <div
                            className='sidebar-item'
                            key={singleEvent.year || index}
                            onClick={() => onEventSelect(singleEvent)}
                            role="button"
                            tabIndex={0}
                            onKeyDown={(e) => {
                                if (e.key === 'Enter' || e.key === ' ') {
                                    e.preventDefault();
                                    onEventSelect(singleEvent);
                                }
                            }}
                            aria-label={`Select year ${singleEvent.year}`}>
                            <h3>{singleEvent.year}</h3>
                        </div>
                    ))
                }
            </div>
            <div className='volume-control'>
                <h3>Volume</h3>
                <input
                    type="range"
                    className='volume-slider'
                    min="0"
                    max="21"
                    value={volume}
                    onChange={handleVolumeChange}
                    aria-label="Volume control"
                    aria-valuemin={0}
                    aria-valuemax={21}
                    aria-valuenow={volume}
                />
            </div>
        </div>
    )
}
