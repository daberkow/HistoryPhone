'use client'

import * as React from "react"
import styles from "./page.module.css"
import SideBar from "./sidebar/sidebar"
import MainPage from "./window/mainpage"
import { API_ENDPOINT, i_events, Data, Folder } from "./types"

export default function Home() {
  const [events, setEvents] = React.useState<Data>({folders: []})
  const [selectedEvent, setSelectedEvent] = React.useState<Folder | null>(null);

  React.useEffect(() => {
      const getEvents = async () => {
          const returnedEvents = await fetchEvents()
          setEvents(returnedEvents)
      }
      getEvents()
  }, [])

  // const fetchEvents = async () => {
  //     const res = await fetch('/events.json')
  //     const data = await res.json()
  //     return data
  // }

  const fetchEvents = async (): Promise<Data> => {
    const res = await fetch('/api/years')
    const data: Data = await res.json()

    // Filter out "volume" and sort years ascending
    const filteredAndSorted = data.folders
      .filter(folder => folder.year !== "volume")
      .sort((a, b) => parseInt(a.year) - parseInt(b.year))

    console.log({ folders: filteredAndSorted })
    return { folders: filteredAndSorted }
  }

  const handleEventSelect = (event: Folder) => {
    setSelectedEvent(event);
  };

  return (
    <main className={styles.main}>
      <SideBar passedEvents={events} onEventSelect={handleEventSelect}></SideBar>
      <MainPage selectedEvent={selectedEvent} />
      {/* {events.map((singleEvent, index) =>
        <div key={singleEvent.id || index}>
          <h3>{singleEvent.id}</h3>
        </div>
      )} */}
    </main>
  );
}
