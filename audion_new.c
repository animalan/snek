// /// AUDIO RELATED CODE 

// int samples0[1000] = {...};

// struct audio_clip {
//     // Info about song trying to play
//     int* samples = samples;
//     int num_samples;
//     // Where am I currently in the song
//     int pos = 0;
//     // Flags
//     bool valid = false; // is this currently a valid song
//     bool restart = false; // should I restart when done 
// }

// #define MAX_SOUNDS 10
// // A list of all the currently playing sounds we want to play
// struct audio_clip sounds[MAX_SOUNDS] = {0};

// // Find an entry in sounds array that is free
// struct audio_clip* findEmptySoundSlot() {
//     for (int i = 0; i < MAX_SOUNDS; i++) {
//         if (!sounds[i].valid) {
//             return sounds[i];
//         }
//     }
//     assert(false);
// }

// // Add something to "sounds we want to be playing" and return immediately, don't actually wait for it to play
// void play_sound(int* samples, int num_samples, bool auto_restart) { // Non-blocking call to play sound
//     struct sound_clip* sound = findEmptySoundSlot();
//     sound->samples = samples;
//     sound->num_samples = num_samples;
//     sound->pos = 0;
//     sound->valid = true;
//     sound->restart = auto_restart;
// }


// // Idea: Run this infrequently to fill the audio queue with samples
// void fillSoundQueue() {
//     // While there is space in the audio queue
//     int num_played_samples = 0;
//     while (audiop->warc != 0 && num_played_samples < 500) {
//         // Build up the sample we want to play
//         int sample = 0;
//         bool foundSample = false;
//         // Go over all currently playing audio clips and superimpose their samples together into sample
//         for (int i =0 ; i < MAX_SOUNDS; i++) {
//             if (sounds[i].valid) {
//                 sample += sounds[i].samples[sounds[i].pos];
//                 sounds[i].pos++;
//                 foundSample = true;
//                 // If this particular song is done
//                 if (sounds[i].pos == sounds[i].num_samples) {
//                     if (sounds[i].restart) {
//                         sounds[i].pos = 0;
//                     } else {
//                         sounds[i].valid = false;
//                     }
//                 }
//             }
//         }
//         if (!foundSample) {
//             break;
//         }

//         audiop->ldata = sample;
//         audiop->rdata = sample;
//         num_played_samples++;
//     }
// }


// ///// INSIDE OF MAIN PROGRAM LOOP

// void game_logic() {
//     ....
//     if (bounces off wall) {
//         // play bouncing off wall sound 
//         play_sound(samples0, sizeof(samples0), false);
//     }

// }

// void setup_interrupts() {
//     All the boilerplate interrupt code we give you 
//     Also, setup the audio device with whatever control flag it needs
//     to know to generate interrupts whenever the output fifo is 25% empty
// }

// void wait_for_vsync() {
//     while (waiting for vsync status bit to change) {
//         fillSoundQueue();
//     }
// }

// void main() {
//     play_sound(background_soundtrack, sizeof(background_soundtrack), true);
//     setup_interrupts();
//     while (1) {
//         //fillSoundQueue();
//         game_logic();
//         draw_screen();
//         wait_for_vsync();    
//     }    
// }


// //// INTERRUPT HANDLER

// // Interrupt triggers when output queue is 25% empty
// // void audioISR() {
// //     fillSoundQueue();
// // }

// // // ALTERNATIVE: USe the timer interrupt and refill the queue every time the timer goes off
// // void timerISR() {
// //     fillSoundQueue();
// // }