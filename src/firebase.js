

// firebase.js
import { initializeApp } from 'firebase/app';
import { getAuth, GoogleAuthProvider } from 'firebase/auth';

const firebaseConfig = {
 apiKey: "AIzaSyD2a1kWmmhM1HTBVZB7KWrnkob-_C6H6ac",
    authDomain: "biopulse-7375f.firebaseapp.com",
    projectId: "biopulse-7375f",
    storageBucket: "biopulse-7375f.firebasestorage.app",
    messagingSenderId: "273781972941",
    appId: "1:273781972941:web:773acc1c6667ea54972e84",
    measurementId: "G-8M6RH3YYGK"
};

const app = initializeApp(firebaseConfig);
const auth = getAuth(app);
const provider = new GoogleAuthProvider();

export { auth, provider };
