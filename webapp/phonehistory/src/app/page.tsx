'use client'

import * as React from "react"
import styles from "./page.module.css"
import SideBar from "./sidebar/sidebar"
import MainPage from "./window/mainpage"
import { i_events } from "./types"

export default function Home() {
  const [events, setEvents] = React.useState<i_events[]>([])

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

  const fetchEvents = async (): Promise<i_events[]> => {
    const res = await fetch('/events.json')
    const data = await res.json()
    return data
  }

  return (
    <main className={styles.main}>
      <SideBar passedEvents={events}></SideBar>
      <MainPage />
      {events.map((singleEvent, index) =>
        <div>
          <h3>{singleEvent.id}</h3>
        </div>
      )}
    </main>
  );
}
