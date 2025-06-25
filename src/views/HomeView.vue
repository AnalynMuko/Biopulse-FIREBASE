<template>
  <v-app>
    <template v-if="!isAuthenticated">
      <v-container class="fill-height" fluid>
        <v-row align="center" justify="center">
          <v-col cols="12" sm="8" md="4">
            <v-card class="pa-4">
              <div class="text-h5 mb-4 text-center">{{ authForm.isSignup ? 'Sign Up' : 'Log In' }}</div>
              <v-text-field v-model="authForm.email" label="Email" type="email" required></v-text-field>
              <v-text-field v-model="authForm.password" label="Password" type="password" required></v-text-field>
              <v-btn color="pink darken-1" dark block class="mb-2" @click="handleAuth">
                {{ authForm.isSignup ? 'Sign Up' : 'Log In' }}
              </v-btn>
              <v-btn text small @click="authForm.isSignup = !authForm.isSignup">
                {{ authForm.isSignup ? 'Already have an account? Log in' : "Don't have an account? Sign up" }}
              </v-btn>
              <v-alert type="error" v-if="authForm.error" dense class="mt-2">
                {{ authForm.error }}
              </v-alert>
            </v-card>
          </v-col>
        </v-row>
      </v-container>
    </template>

    <template v-else>
      <v-navigation-drawer app v-model="drawer" color="pink darken-1" dark>
        <v-list>
          <v-list-item class="text-h6">BioPulseeeeeeeeeeeeee</v-list-item>
          <v-list-item
            v-for="item in menuItems"
            :key="item.text"
            link
            @click="handleMenuClick(item.text)"
          >
            <v-list-item-content>{{ item.text }}</v-list-item-content>
          </v-list-item>
        </v-list>
      </v-navigation-drawer>

      <v-app-bar app color="white" flat>
        <v-app-bar-nav-icon @click="drawer = !drawer"></v-app-bar-nav-icon>
        <v-toolbar-title class="text-pink--text text--darken-1">BioPulse</v-toolbar-title>
      </v-app-bar>

      <v-main>
        <v-container>
          <div v-if="currentPage === 'Dashboard'">
            <v-row class="mb-4">
              <v-col cols="12" sm="6" md="3" v-for="card in cards" :key="card.title">
                <v-card color="pink darken-1" dark class="pa-4">
                  <div class="text-h6">{{ card.title }}</div>
                  <div class="text-h4 font-weight-bold">{{ card.value }}</div>
                  <div class="text-subtitle-2">{{ card.status }}</div>
                </v-card>
              </v-col>
            </v-row>

            <v-text-field
              v-model="search"
              label="Search User"
              prepend-inner-icon="mdi-magnify"
              dense
              outlined
            ></v-text-field>

            <v-btn class="mb-2" color="primary" @click="fetchDataFromFirebase">
              Fetch Data from Firebase
            </v-btn>

            <div v-if="firebaseUsersData.length > 0">
              <div class="mb-1">
                <strong>Temperature:</strong> {{ latestTemperature }}
              </div>
              <div class="mb-1">
                <strong>Blood Pressure:</strong> {{ latestBloodPressure }}
              </div>
            </div>

            <v-data-table
              :headers="headers"
              :items="firebaseUsersData.length > 0 ? [firebaseUsersData[firebaseIndex]] : []"
              :items-per-page="1"
              class="elevation-1"
            >
              <template v-slot:item.status="{ item }">
                <v-chip :color="getStatusColor(item.status)" dark small>
                  {{ item.status }}
                </v-chip>
              </template>
              <template v-slot:bottom>
                <div class="d-flex justify-end">
                  <v-btn
                    v-if="firebaseUsersData.length > 1"
                    :disabled="firebaseIndex === 0"
                    class="mr-2"
                    @click="previousData"
                  >
                    Previous
                  </v-btn>
                  <v-btn
                    v-if="firebaseUsersData.length > 0"
                    :disabled="firebaseIndex === firebaseUsersData.length - 1"
                    @click="nextData"
                  >
                    Next
                  </v-btn>
                </div>
              </template>
            </v-data-table>
          </div>

          <div v-if="currentPage === 'Settings'">
            <v-card class="pa-4" color="grey lighten-4" outlined>
              <div class="text-h6 mb-2">ESP32 Status</div>
              <v-alert :type="espStatus.connected ? 'success' : 'error'" dense>
                ESP32: {{ espStatus.message }}
              </v-alert>
              <v-btn color="primary" @click="checkESPConnection">Check Connection</v-btn>
            </v-card>
          </div>
        </v-container>
      </v-main>
    </template>
  </v-app>
</template>

<script>
// Import Firebase modules
import { initializeApp } from 'firebase/app';
import { getFirestore, collection, getDocs } from 'firebase/firestore';

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyD2a1kWmmhM1HTBVZB7KWrnkob-_C6H6ac",
  authDomain: "biopulse-7375f.firebaseapp.com",
  projectId: "biopulse-7375f",
  storageBucket: "biopulse-7375f.firebasestorage.app",
  messagingSenderId: "273781972941",
  appId: "1:273781972941:web:773acc1c6667ea54972e84",
  measurementId: "G-8M6RH3YYGK"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const db = getFirestore(app);

export default {
  data() {
    return {
      drawer: true,
      isAuthenticated: false,
      currentPage: 'Dashboard',
      search: '',
      menuItems: [
        { text: 'Dashboard' },
        { text: 'Sensor Data' },
        { text: 'Reports' },
        { text: 'Settings' },
        { text: 'User Profile' },
        { text: 'Notifications' },
        { text: 'Logout' },
      ],
      authForm: {
        email: '',
        password: '',
        isSignup: false,
        error: '',
      },
      cards: [
        { title: 'Heart Rate', value: '78 bpm', status: 'Normal' },
        { title: 'Oxygen Saturation', value: '98%', status: 'Optimal' },
        { title: 'Body Temperature', value: '36.7°C', status: 'Stable' },
        { title: 'Blood Pressure', value: '120/80 mmHg', status: 'Normal' },
      ],
      headers: [
        { text: 'Name', value: 'name' },
        { text: 'Heart Rate', value: 'heartRate' },
        { text: 'Oxygen', value: 'oxygen' },
        { text: 'Temperature', value: 'temperature' },
        { text: 'Blood Pressure', value: 'bloodPressure' }, // Added for DataTable
        { text: 'Status', value: 'status' },
      ],
      users: [
        {
          name: 'Iman',
          heartRate: '75 bpm',
          oxygen: '97%',
          temperature: '36.6°C',
          bloodPressure: '118/79',
          status: 'Stable',
        },
        {
          name: 'Aiza',
          heartRate: '90 bpm',
          oxygen: '95%',
          temperature: '37.2°C',
          bloodPressure: '130/85',
          status: 'Slightly Elevated',
        },
        {
          name: 'Jenny',
          heartRate: '110 bpm',
          oxygen: '92%',
          temperature: '38.5°C',
          bloodPressure: '140/90',
          status: 'Critical',
        },
        {
          name: 'Bea',
          heartRate: '120 bpm',
          oxygen: '95%',
          temperature: '39.5°C',
          bloodPressure: '150/90',
          status: 'Critical',
        },
      ],
      espStatus: {
        connected: false,
        message: 'Not Connected',
      },
      firebaseUsersData: [],
      latestTemperature: '',
      latestBloodPressure: '',
      firebaseIndex: 0,
    };
  },
  computed: {
    filteredUsers() {
      if (!this.search) return this.users;
      return this.users.filter((user) =>
        user.name.toLowerCase().includes(this.search.toLowerCase())
      );
    },
  },
  mounted() {
    const user = localStorage.getItem('user');
    if (user) this.isAuthenticated = true;
  },
  methods: {
    getStatusColor(status) {
      switch (status) {
        case 'Stable':
          return 'green';
        case 'Slightly Elevated':
          return 'orange';
        case 'Critical':
          return 'red';
        default:
          return 'grey';
      }
    },
    handleMenuClick(text) {
      if (text === 'Logout') {
        localStorage.removeItem('user');
        this.isAuthenticated = false;
        this.authForm.email = '';
        this.authForm.password = '';
        this.authForm.error = '';
      } else {
        this.currentPage = text;
      }
    },
    handleAuth() {
      const { email, password, isSignup } = this.authForm;
      const storedUser = JSON.parse(localStorage.getItem('user'));

      if (!email || !password) {
        this.authForm.error = 'Please enter email and password.';
        return;
      }

      if (isSignup) {
        if (storedUser && storedUser.email === email) {
          this.authForm.error = 'Email already exists.';
        } else {
          localStorage.setItem('user', JSON.stringify({ email, password }));
          this.isAuthenticated = true;
        }
      } else {
        if (!storedUser || storedUser.email !== email || storedUser.password !== password) {
          this.authForm.error = 'Invalid email or password.';
        } else {
          this.isAuthenticated = true;
        }
      }
    },
    async checkESPConnection() {
      try {
        const response = await fetch('http://192.168.1.100/status'); // Replace with your ESP32's IP
        const data = await response.json();
        if (data.connected) {
          this.espStatus.connected = true;
          this.espStatus.message = 'Connected Successfully';
        } else {
          this.espStatus.connected = false;
          this.espStatus.message = 'Not Connected';
        }
      } catch (error) {
        this.espStatus.connected = false;
        this.espStatus.message = 'Not Connected';
      }
    },
    async fetchDataFromFirebase() {
      try {
        const querySnapshot = await getDocs(collection(db, "lalala"));
        this.firebaseUsersData = querySnapshot.docs.map(doc => {
          const data = doc.data();
          return {
            ...data,
            // Ensure these fields exist in your Firebase documents
            name: data.name || 'N/A',
            heartRate: data.heartRate ? `${data.heartRate} bpm` : 'N/A',
            oxygen: data.oxygen ? `${data.oxygen}%` : 'N/A',
            temperature: data.temperature ? `${data.temperature}°C` : 'N/A',
            bloodPressure: data.systolic_bp && data.diastolic_bp ? `${data.systolic_bp}/${data.diastolic_bp}` : 'N/A',
            status: data.status || 'Unknown'
          };
        });
        this.firebaseIndex = 0;
        this.updateLabels();
      } catch (error) {
        console.error("Error fetching data from Firebase:", error);
      }
    },
    updateLabels() {
      if (this.firebaseUsersData.length > 0) {
        const currentEntry = this.firebaseUsersData[this.firebaseIndex];
        this.latestTemperature = currentEntry.temperature ? `${currentEntry.temperature}` : 'N/A';
        this.latestBloodPressure = currentEntry.bloodPressure || 'N/A'; // Use the formatted bloodPressure
      } else {
        this.latestTemperature = 'N/A';
        this.latestBloodPressure = 'N/A';
      }
    },
    nextData() {
      if (this.firebaseIndex < this.firebaseUsersData.length - 1) {
        this.firebaseIndex++;
      }
    },
    previousData() {
      if (this.firebaseIndex > 0) {
        this.firebaseIndex--;
      }
    },
  },
  watch: {
    currentPage(newVal) {
      if (newVal === 'Settings') {
        this.checkESPConnection();
      }
    },
    firebaseIndex() {
      this.updateLabels();
    },
  },
};
</script>

<style scoped>
.v-navigation-drawer {
  width: 220px;
}
.v-card {
  border-radius: 12px;
}
</style>