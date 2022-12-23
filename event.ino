// обозначаем все пины, где подключены элементы именами
#define motor 2
#define stopButton 3
#define startButton 4
#define abortButton 5
#define resumeButton 6
#define redLed 7
#define yellowLed 8
#define greenLed 9

//таймер работы в миллисекундах (изначально 30с или 30000мс)
int workTimerMS = 30000;
// таймер моргания светодиода (в мс)
short blinkTime = 300;
// состояние, где 0 - ожидание, 1 - работа, 2 - остановка, 3 - авария
char state = 0;

void lightLED(char led) { //функция для управления светодиодной системой
	//сначала сброс всех светодиодов
	digitalWrite(redLed, 0);
	digitalWrite(yellowLed, 0);
	digitalWrite(greenLed, 0);
	//включение нужного, 1 - красный, 2 - желтый, 3 - зеленый
	if(led == 1) {
		digitalWrite(redLed, 1);
	} else if(led == 2) {
		digitalWrite(yellowLed, 1);
	} else if(led == 3) {
		digitalWrite(greenLed, 1);
	}
}

void setup()
{
	// выставляем режимы пинов
	pinMode(motor, OUTPUT);
	pinMode(stopButton, INPUT);
	pinMode(startButton, INPUT);
	pinMode(abortButton, INPUT);
	pinMode(resumeButton, INPUT);
	pinMode(redLed, OUTPUT);
	pinMode(yellowLed, OUTPUT);
	pinMode(greenLed, OUTPUT);
  	Serial.begin(9600);
}

void loop()
{
	if(digitalRead(abortButton) == 1) { // если нажата кнопка "авария"
		state = 3; // независимо от состояния меняем его на "авария"
	}

	if(state == 0) { //если состояние ожидания
		if(workTimerMS == 0) { //если таймер работы закончился, то сбрасываем
			workTimerMS = 30000;
		}
		lightLED(2); // зажигаем желтый светодиод, "ждем"
		if(digitalRead(startButton) == 1) { // если кнопка "старт" нажата
			state = 1; // меняем состояние на "работа"
		}
	}

	if(state == 1) { // если состояние работа
		digitalWrite(motor, 1); // стартуем мотор
		lightLED(3);
		if(workTimerMS > 0) { // если в таймере отсчета есть время
			if(digitalRead(stopButton) == 1) { // если нажата кнопка стоп 
				digitalWrite(motor,0); // выключаем мотор
				state = 2; // возвращаем состояние в "паузу"
			}
			workTimerMS -= 1; // отсчитываем 1
			delay(1); // ждем одну миллисекунду
        } else if(workTimerMS <= 0) {// если время работы закончилось
			digitalWrite(motor,0); // выключаем мотор
			state = 0; // возвращаем состояние в "ожидание"
		}
	}

	if(state == 2) { // если пауза
		lightLED(-1); // выключить все светодиоды
		// моргать светодиодом (300мс вкл 300мс выкл)
		if(blinkTime > 0) {
			digitalWrite(yellowLed, 1);
		} else if(blinkTime < 0) {
			digitalWrite(yellowLed, 0);
		}
		if(blinkTime < -300) {
			blinkTime = 300;
		}
		blinkTime -= 100;
		delay(100);
		if(digitalRead(stopButton) == 1) { // если кнопка стоп нажата
			state = 0; // состояние "ожидание"
			workTimerMS = 30000; // таймер работы сброшен
		}
		if(digitalRead(startButton) == 1) { // если кнопка старт нажата
			state = 1; // возвращаем на работу (состояние "работа")
		}
	}

	if(state == 3) { // если у нас авария
		digitalWrite(motor, 0); //останавливаем мотор
		lightLED(1); // включаем красный светодиод
		if(digitalRead(resumeButton) == 1) {
			state = 2;
		}
	}
}

// made by RodionGromo