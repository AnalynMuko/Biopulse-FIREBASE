<template>
    <v-container class="fill-height" fluid>
      <v-row align="center" justify="center">
        <v-col cols="12" md="4">
          <v-card class="pa-6" color="pink lighten-4">
            <v-card-title class="text-h5 text-center">Login</v-card-title>
            <v-card-text>
              <v-btn color="pink" dark block @click="login">
                Login with Google
              </v-btn>
            </v-card-text>
          </v-card>
        </v-col>
      </v-row>
    </v-container>
  </template>
  
  <script setup>
  import { ref } from 'vue'
  import { useRouter } from 'vue-router'
  import { signInWithPopup } from 'firebase/auth'
  import { auth, provider } from '@/firebase'
  
  const router = useRouter()
  
  const login = async () => {
    try {
      const result = await signInWithPopup(auth, provider)
      console.log('Logged in:', result.user)
      router.push('/') // after login, go back to dashboard
    } catch (error) {
      console.error('Login failed:', error)
    }
  }
  </script>
  