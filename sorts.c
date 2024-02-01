#if defined(_FEATHER_M4_) // Feather M4 + RGB Matrix FeatherWing
  uint8_t rgbPins[]  = {6, 5, 9, 11, 10, 12};
  uint8_t addrPins[] = {A5, A4, A3, A2};
  uint8_t clockPin   = 13;
  uint8_t latchPin   = 0;
  uint8_t oePin      = 1;
#elif defined(__SAMD51__) // M4 Metro Variants (Express, AirLift)
  uint8_t rgbPins[]  = {6, 5, 9, 11, 10, 12};
  uint8_t addrPins[] = {A5, A4, A3, A2};
  uint8_t clockPin   = 13;
  uint8_t latchPin   = 0;
  uint8_t oePin      = 1;
#elif defined(_SAMD21_) // Feather M0 variants
  uint8_t rgbPins[]  = {6, 7, 10, 11, 12, 13};
  uint8_t addrPins[] = {0, 1, 2, 3};
  uint8_t clockPin   = SDA;
  uint8_t latchPin   = 4;
  uint8_t oePin      = 5;
#elif defined(NRF52_SERIES) // Special nRF52840 FeatherWing pinout
  uint8_t rgbPins[]  = {6, A5, A1, A0, A4, 11};
  uint8_t addrPins[] = {10, 5, 13, 9};
  uint8_t clockPin   = 12;
  uint8_t latchPin   = PIN_SERIAL1_RX;
  uint8_t oePin      = PIN_SERIAL1_TX;
#elif defined(ESP32)
  // 'Safe' pins, not overlapping any peripherals:
  // GPIO.out: 4, 12, 13, 14, 15, 21, 27, GPIO.out1: 32, 33
  // Peripheral-overlapping pins, sorted from 'most expendible':
  // 16, 17 (RX, TX)
  // 25, 26 (A0, A1)
  // 18, 5, 9 (MOSI, SCK, MISO)
  // 22, 23 (SCL, SDA)
  uint8_t rgbPins[]  = {4, 12, 13, 14, 15, 21};
  uint8_t addrPins[] = {16, 17, 25, 26};
  uint8_t clockPin   = 27; // Must be on same port as rgbPins
  uint8_t latchPin   = 32;
  uint8_t oePin      = 33;
#elif defined(ARDUINO_TEENSY40)
  uint8_t rgbPins[]  = {15, 16, 17, 20, 21, 22}; // A1-A3, A6-A8, skip SDA,SCL
  uint8_t addrPins[] = {2, 3, 4, 5};
  uint8_t clockPin   = 23; // A9
  uint8_t latchPin   = 6;
  uint8_t oePin      = 9;
#elif defined(ARDUINO_TEENSY41)
  uint8_t rgbPins[]  = {26, 27, 38, 20, 21, 22}; // A12-14, A6-A8
  uint8_t addrPins[] = {2, 3, 4, 5};
  uint8_t clockPin   = 23; // A9
  uint8_t latchPin   = 6;
  uint8_t oePin      = 9;
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
  // RP2040 support requires the Earle Philhower board support package;
  // will not compile with the Arduino Mbed OS board package.
  // The following pinout works with the Adafruit Feather RP2040 and
  // original RGB Matrix FeatherWing (M0/M4/RP2040, not nRF version).
  // Pin numbers here are GP## numbers, which may be different than
  // the pins printed on some boards' top silkscreen.
  uint8_t rgbPins[]  = {8, 7, 9, 11, 10, 12};
  uint8_t addrPins[] = {25, 24, 29, 28};
  uint8_t clockPin   = 13;
  uint8_t latchPin   = 1;
  uint8_t oePin      = 0;
#endif

/* ----------------------------------------------------------------------
Matrix initialization is explained EXTENSIVELY in "simple" example sketch!
It's very similar here, but we're passing "true" for the last argument,
enabling double-buffering -- this permits smooth animation by having us
draw in a second "off screen" buffer while the other is being shown.
------------------------------------------------------------------------- */

Adafruit_Protomatter matrix(
  256,          // Matrix width in pixels
  5,           // Bit depth -- 6 here provides maximum color options
  1, rgbPins,  // # of matrix chains, array of 6 RGB pins for each
  4, addrPins, // # of address pins (height is inferred), array of pins
  clockPin, latchPin, oePin, // Other matrix control pins
  true);       // HERE IS THE MAGIG FOR DOUBLE-BUFFERING!

// Data

#define N_NUMBERS 128
#define HEIGHT 32
uint8_t numbers[N_NUMBERS];

const char* name_bubble = "bubble sort";
const char* name_merge = "merge sort";
const char* name_quick = "quick sort";
const char* name_bogo = "bogo sort";
const char* name_radix = "radix sort";
const char* current_name = NULL;

// data functions

void shuffle_numbers() {
  for (int i=0; i < N_NUMBERS; i++) {
    uint8_t left = random(N_NUMBERS);
    uint8_t right = random(N_NUMBERS);
    // swap
    uint8_t tmp = numbers[left];
    numbers[left] = numbers[right];
    numbers[right] = tmp;
  }

  // debug
  for (int i=0; i < N_NUMBERS; i++) {
    Serial.print(numbers[i]);
    Serial.print(", ");
  }
  Serial.println();
}

// drawing functions

void render_line(uint16_t x) {
  // map 1-256 range to fit on screen
  uint16_t render_height = 1 + numbers[x] / (N_NUMBERS / HEIGHT);

  uint16_t hue = (numbers[x] * 2) << 8;

  // draw function draws from top to bottom
  matrix.drawFastVLine(x*2, HEIGHT-render_height, render_height, matrix.colorHSV(hue,255,255));
  matrix.drawFastVLine(x*2 + 1, HEIGHT-render_height, render_height, matrix.colorHSV(hue,255,255));
  // or a dot
//  matrix.drawPixel(x, HEIGHT-render_height, matrix.colorHSV(hue,255,255));
}

void frame() {
  matrix.fillScreen(0);
  render();
  matrix.show();
//  delay(1);
}

void wait(int frames) {
   for (int i=0; i<frames; i++) {
    frame();
  }
}

void render() {
  if (current_name != NULL) {
    matrix.setCursor(1, 0);
    matrix.setTextSize(1);
    matrix.setTextColor(0xFFFF);
    matrix.println(current_name);
  }
  for (int i=0; i < N_NUMBERS; i++) {
    render_line(i);
  }
}

// sorts

void animate_bubble_sort() {
  wait(240);

  bool changed = true;
  
  while (changed) {
    changed = false;
    for (int j=0; j<N_NUMBERS - 1; j++) {
      if (numbers[j] > numbers[j+1]) {
        uint8_t tmp = numbers[j];
        numbers[j] = numbers[j+1];
        numbers[j+1] = tmp;
        frame();
        changed = true;
        break;
      }
    }
  }

  wait(240);
}

void merge(uint8_t arr[], int start, int mid, int end) {
  int start2 = mid + 1;

  // If the direct merge is already sorted
  if (arr[mid] <= arr[start2]) {
    return;
  }

  // Two pointers to maintain start
  // of both arrays to merge
  while (start <= mid && start2 <= end) {

    // If element 1 is in right place
    if (arr[start] <= arr[start2]) {
      start++;
    }
    else {
      int value = arr[start2];
      int index = start2;

      // Shift all the elements between element 1
      // element 2, right by 1.
      while (index != start) {
        arr[index] = arr[index - 1];
        frame();
        index--;
      }
      arr[start] = value;
      frame();

      // Update all the pointers
      start++;
      mid++;
      start2++;
    }
  }
}

void merge_sort(uint8_t arr[], int l, int r) {
  if (l < r) {

    // Same as (l + r) / 2, but avoids overflow
    // for large l and r
    int m = l + (r - l) / 2;

    // Sort first and second halves
    merge_sort(arr, l, m);
    merge_sort(arr, m + 1, r);

    merge(arr, l, m, r);
  }
}

void animate_merge_sort() {
  wait(240);
  merge_sort(numbers, 0, N_NUMBERS-1);
  wait(240);
}

void quicksort(uint8_t *nums, int len) {
  if (len < 2) return;
 
  int pivot = nums[len / 2];
 
  int i, j;
  for (i = 0, j = len - 1; ; i++, j--) {
    while (nums[i] < pivot) i++;
    while (nums[j] > pivot) j--;
 
    if (i >= j) break;
 
    int temp = nums[i];
    nums[i]  = nums[j];
    nums[j]  = temp;
    frame();
    frame();
  }
 
  quicksort(nums, i);
  quicksort(nums + i, len - i);
}

void animate_bogo_sort() {
  wait(240);
  for (int i=0; i < 1024; i++) {
    shuffle_numbers();
    frame();
    bool sorted = true;
    for (int j=0; j < N_NUMBERS-1; j++) {
      if (numbers[j] > numbers[j+1]) {
        sorted = false;
        break;
      }
    }
    if (sorted == true) {
      break;
    }
  }
  wait(240);
}

void animate_quick_sort() {
  wait(240);
  quicksort(numbers, N_NUMBERS);
  frame();
  wait(240);
}

/* sort unsigned ints */
static void rad_sort_u(uint8_t *from, uint8_t *to, uint8_t bit) {
  if (!bit || to < from + 1) return;
 
  uint8_t *ll = from, *rr = to - 1;
  for (;;) {
    /* find left most with bit, and right most without bit, swap */
    while (ll < rr && !(*ll & bit)) ll++;
    while (ll < rr &&  (*rr & bit)) rr--;
    if (ll >= rr) break;
    uint8_t tmp = *ll;
    *ll = *rr;
    *rr = tmp;
    frame();
    frame();
  }
 
  if (!(bit & *ll) && ll < to) ll++;
  bit >>= 1;
 
  rad_sort_u(from, ll, bit);
  rad_sort_u(ll, to, bit);
}

void animate_radix_sort() {
  wait(240);
  rad_sort_u(numbers, numbers+N_NUMBERS, 0x80);
  frame();
  wait(240);
}

// SETUP - RUNS ONCE AT PROGRAM START --------------------------------------

void setup(void) {
  Serial.begin(9600);

  // Initialize matrix...
  ProtomatterStatus status = matrix.begin();
  Serial.print("Protomatter begin() status: ");
  Serial.println((int)status);
  if(status != PROTOMATTER_OK) {
    // DO NOT CONTINUE if matrix setup encountered an error.
    for(;;);
  }

  // init numbers
  for (int i=0; i<N_NUMBERS; i++) {
    numbers[i] = i;
  }

  // buzzer
  MCLK->APBAMASK.reg |= MCLK_APBAMASK_TC0;           // Activate timer TC0
  
  // Set up the generic clock (GCLK7) used to clock timers 
  GCLK->GENCTRL[7].reg = GCLK_GENCTRL_DIV(3) |       // Divide the 48MHz clock source by divisor 3: 48MHz/3 = 16MHz
                         GCLK_GENCTRL_IDC |          // Set the duty cycle to 50/50 HIGH/LOW
                         GCLK_GENCTRL_GENEN |        // Enable GCLK7
                         GCLK_GENCTRL_SRC_DFLL;      // Generate from 48MHz DFLL clock source
  while (GCLK->SYNCBUSY.bit.GENCTRL7);               // Wait for synchronization  

  GCLK->PCHCTRL[9].reg = GCLK_PCHCTRL_CHEN |         // Enable perhipheral channel
                         GCLK_PCHCTRL_GEN_GCLK7;     // Connect generic clock 7 to TC0

  // Enable the peripheral multiplexer on pin A1
  PORT->Group[g_APinDescription[A1].ulPort].PINCFG[g_APinDescription[A1].ulPin].bit.PMUXEN = 1;
  
  // Set A1 the peripheral multiplexer to peripheral E(4): TC0, Channel 1
  PORT->Group[g_APinDescription[A1].ulPort].PMUX[g_APinDescription[A1].ulPin >> 1].reg |= PORT_PMUX_PMUXO(4);
  
  TC0->COUNT16.CTRLA.reg = TC_CTRLA_PRESCALER_DIV16 |        // Set prescaler to 16, 16MHz/16 = 1MHz
                           TC_CTRLA_PRESCSYNC_PRESC |        // Set the reset/reload to trigger on prescaler clock
                           TC_CTRLA_MODE_COUNT16;            // Set the counter to 16-bit mode

  TC0->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MPWM;      // Set-up TC0 timer for Match PWM mode (MPWM)

  TC0->COUNT16.CC[0].reg = 19999;                    // Use CC0 register as TOP value, set for 50Hz PWM
  while (TC0->COUNT16.SYNCBUSY.bit.CC0);             // Wait for synchronization

  TC0->COUNT16.CC[1].reg = 9999;                     // Set the duty cycle to 50% (CC1 half of CC0)
  while (TC0->COUNT16.SYNCBUSY.bit.CC1);             // Wait for synchronization

  TC0->COUNT16.CTRLA.bit.ENABLE = 1;                 // Enable timer TC0
  while (TC0->COUNT16.SYNCBUSY.bit.ENABLE);          // Wait for synchronization
}

// LOOP - RUNS REPEATEDLY AFTER SETUP --------------------------------------

void loop(void) {
    Serial.println("bubble");
    shuffle_numbers();
    current_name = name_bubble;
    animate_bubble_sort();
  
    Serial.println("merge");
    shuffle_numbers();
    current_name = name_merge;
    animate_merge_sort();
  
    Serial.println("quick");
    shuffle_numbers();
    current_name = name_quick;
    animate_quick_sort();

    Serial.println("radix");
    shuffle_numbers();
    current_name = name_radix;
    animate_radix_sort();

    Serial.println("bogo");
    shuffle_numbers();
    current_name = name_bogo;
    animate_bogo_sort();
}
