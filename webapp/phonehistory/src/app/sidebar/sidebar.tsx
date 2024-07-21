import * as React from "react"
import './sidebar.css'
import Link from 'next/link'
import { i_events } from "../types"

interface SidebarProps {
    passedEvents: i_events[];
}

export default function Sidebar({ passedEvents }: { passedEvents: i_events[] }) {
    return (
        <div className='sidebar'>
            <h4>Events</h4>
            <hr></hr>
            {
                passedEvents.map((singleEvent) => (
                    <div>
                        <h3>{singleEvent.title}</h3>
                    </div>
                ))
            }
        </div>
    )
}
