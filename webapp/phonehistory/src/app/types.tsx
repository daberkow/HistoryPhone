export interface i_events {
    id: number,
    title: string,
    descr: string,
    narrator: number
}

export interface Folder {
    year: string;
    photos: number;
    text_documents: number;
    mp3_files: number;
}

export interface Data {
    folders: Folder[];
}

export const API_ENDPOINT = 'http://192.168.4.1';
