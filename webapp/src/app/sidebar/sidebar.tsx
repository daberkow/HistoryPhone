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
    const [currentYear, setCurrentYear] = React.useState<string>(''); // Current year from /api/current

    // Periodic check for current year
    React.useEffect(() => {
        const fetchCurrentYear = async () => {
            try {
                const response = await fetch('/api/current');
                if (response.ok) {
                    const year = await response.text();
                    if (year == '0') {
                        setCurrentYear('None');
                    } else {
                        setCurrentYear(year);
                    }
                }
            } catch (error) {
                console.error('Error fetching current year:', error);
            }
        };

        // Fetch immediately on mount
        fetchCurrentYear();

        // Set up periodic polling every 1 second
        const intervalId = setInterval(fetchCurrentYear, 1000);

        // Cleanup interval on unmount
        return () => clearInterval(intervalId);
    }, []);

    const handleCurrentYearClick = () => {
        if (currentYear && currentYear !== 'None') {
            const yearNumber = parseInt(currentYear, 10);
            const matchingEvent = passedEvents.folders.find(
                folder => folder.year?.toString() === currentYear
            );
            if (matchingEvent) {
                onEventSelect(matchingEvent);
            }
        }
    };

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
            <div className='current-year-display'>
                <h3>Currently Playing</h3>
                <div
                    className={`current-year-value ${currentYear && currentYear !== 'None' ? 'clickable' : ''}`}
                    onClick={handleCurrentYearClick}
                    role={currentYear && currentYear !== 'None' ? 'button' : undefined}
                    tabIndex={currentYear && currentYear !== 'None' ? 0 : undefined}
                    onKeyDown={(e) => {
                        if (currentYear && currentYear !== 'None' && (e.key === 'Enter' || e.key === ' ')) {
                            e.preventDefault();
                            handleCurrentYearClick();
                        }
                    }}
                    aria-label={currentYear && currentYear !== 'None' ? `Go to year ${currentYear}` : undefined}
                >
                    {currentYear || 'None'}
                </div>
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
