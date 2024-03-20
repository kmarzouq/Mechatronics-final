void distance_setup() {
  pinMode(A3, INPUT);
  // Serial.begin(9600);
}



// float points[] = {580, 412, 305, 241, 201, 171, 144, 130};
float points[] = {660, 519, 340, 290, 240, 210, 195, 175};
float avg_slope = -14.343;
// float avg_slope = -12.857;

int countd = 0;
float total = 0;
float distances[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int di = 0;
const int SAMPLES = 10;

// todo: points were innacurate in maker lab, account for lighting conditions?
float measure_distance() {
  float distance;
  distances[di] = analogRead(A3);
  float sum = 0;
  for (int i = 0; i < SAMPLES; i++)
    sum += distances[i];

  int avg = sum/SAMPLES;
  // Serial.println(avg);
  di = (di + 1) % SAMPLES;

  int i = 0;
  // Serial.print(avg);
  // Serial.print(" ");
  while(avg <= points[i] && i <= 8) i++;

  float slope;
  if (i <= 0)
    distance = 5 - (points[0] - avg) / avg_slope;
  else if(i >= 8)
    distance = 40 + (avg - points[7]) / avg_slope;
  else {
    slope = (points[i] - points[i-1])/5;
    distance = (avg - points[i-1]) / slope + i*5;
  }
  return distance;
  // Serial.println(distance);
}
