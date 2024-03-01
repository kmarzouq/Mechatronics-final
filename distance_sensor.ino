void setup() {
    pinMode(A3, INPUT);
    Serial.begin(9600);
}


bool measure = true;

float points[] = {580, 412, 305, 241, 201, 171, 144, 130};
float avg_slope = -12.857;

int count = 0;
float total = 0; 

void loop() {
  if (count == 9) {
    float analog = total/10.0;
    // Serial.println(analog);
    total = analogRead(A3);
    count = 1;

    int i = 0;
    while(analog <= points[i] && i <= 8) i++;

    float slope;
    if (i <= 0) {
      Serial.println(5 - (points[0] - analog) / avg_slope);
    } else {
      if (i >= 8)
        slope = avg_slope;
      else
        slope = (points[i] - points[i-1])/5;

      Serial.println((analog - points[i-1]) / slope + i*5);
    }

  } else {
    count++;
    total += analogRead(A3);
  }
}

bool aeq(float expect, float actual) {
  return abs(actual - expect)/expect <= 0.1;
}
