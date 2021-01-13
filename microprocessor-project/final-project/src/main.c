#include <stm32l476xx.h>
#include <utils.h>
#include <stdlib.h> /* 亂數相關函數 */
#define SET_REG(REG,SELECT,VAL) {((REG)=((REG)&(~(SELECT))) | (VAL));};

#define C3 131
#define D3 147
#define E3 165
#define F3 175
#define G3 196
#define A3 220
#define B3 247
#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define A4 440
#define B4 494
#define C5 523
#define D5 587
#define E5 659
#define F5 698
#define G5 784
#define A5 880
#define B5 988
#define C6 1047


// PB0 - A3   PB1 - CN10最右下數8   PB2 - CN10最右下數9   PB3 - D3
// PB4 - D5   PB5 - D4     PB6 - D10       PB7 - CN7最左下數9
int keypad_output[] = {0, 1, 2, 3};	// GPIOB
int keypad_input[] = {4, 5, 6, 7};	// GPIOB
// PC0 - A5   PC1 - A4   PC2 - CN7最左下數2
int max7219[] = {0, 1, 2};			// GPIOC
// PC3 - CN7最左下數1   PC4 - CN10最右下數3
// PC5 - CN10最右上數3   PC6 - CN10最右上數2
//int led[] = {6};			// GPIOC
int led[] = {3, 4, 5, 6};			// GPIOC
int button[] = {13};				// GPIOC
// PA6 - D12   PA7 - D11
// PA8 - D7   PA9 - D8
int sensor[] = {6, 7, 8, 9};		// GPIOA
// PA5 - D13
int beep[] = {5}; // GPIOA
// for interrupt
int sensor_interrupt[] = {23};		// EXTI9_5 in NVIC

extern void max7219_init(int decode_mode);	// 0x00: no-decode, 0xff: code b
extern void MAX7219Send(unsigned char address, unsigned char data);

void GPIO_init();
void Delay();
void show8(int number);
void showScore(int score);

// for interrupt
// void NVIC_config();
// void EXTI_config();
void Timer_init();
void PWM_channel_init();
// void EXTI9_5_IRQHandler();
// void delay(int msec);

void delay(int msec);
void giveMeBeep(int frequency, int duration);

// 7-segment
// select mode
void showBASIC();
void showFREE();
void showMUSIC();
// void showLEVEL();
// select method to generate led
void showHELP();
void show1245();
// select which game to play
void showGAME(int i);

// 記憶遊戲
void letsHint(int arr[], int n);
void showgogogogo();
void showGoodJob();
void showSEEYOU();

// 打地鼠
void showSPEED(int i);

// 我是神射⼿
void showBALL(int stage, int ballLeft);

// 刷分仔
void showTimeLeft(int stage, int timeLeft);

// MUSIC mode
void showLIVE();
void showREC();

int valueInArray(int value, int arr[], int n);
void setArray(int arr[], int n, int value);


int main()
{
	GPIO_init();
//	max7219_init(0xff);
//	show8(15);
	max7219_init(0x00);
	show8(0b00000000);
	GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);	// 燈先關掉

	// for interrupt
	// NVIC_config();
	// EXTI_config();

	// // test for the sensor
	// // the sensor is active low!!!
	// // https://www.playembedded.org/blog/detecting-obstacle-with-ir-sensor-and-arduino/
	// while (1) {
	// 	if (!(GPIOA->IDR & (1<<9))) {
	// 		GPIOC->ODR = GPIOC->ODR & ~(15 << 3);
	// 	} else {
	// 		GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);	// 燈先關掉
	// 	}
	// }

//	giveMeBeep(262, 1);
//	giveMeBeep(262, 1);
//	giveMeBeep(392, 1);
//	giveMeBeep(392, 1);
//	giveMeBeep(440, 1);
//	giveMeBeep(440, 1);
//	giveMeBeep(392, 1);
	
	// TIM2->CR1 |= TIM_CR1_CEN;	// Start timer
	// Delay(); Delay(); Delay(); Delay(); Delay();
	// Delay(); Delay(); Delay(); Delay(); Delay();
	// TIM2->CR1 &= ~TIM_CR1_CEN; // stop timer
	// TIM2->EGR |= TIM_EGR_UG; // reset timer


	// keypad to select mode
	int mode;
	int tempMode = 0;	// 0: BASIC, 1: FREE, 2: LEVEL
	int Table[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{-1,3,1,0}};
	next0:

	Timer_init();

	tempMode = 0;
	showBASIC();
	while (1) {
		Delay();
		for (int col = 0; col < 4; ++col) {
			GPIOB->ODR = (GPIOB->ODR & ~0xF) | (1<<col);
			for (int row = 0; row < 4; ++row) {
				if (GPIOB->IDR & (1<<(4+row))) {
					if (Table[row][col] == 3) {
						mode = tempMode;
						show8(0b00000000);
						goto next1;
					}
					tempMode = (tempMode + Table[row][col] + 3) % 3;
					switch (tempMode) {
					case 0: showBASIC(); break;
					case 1: showFREE(); break;
					case 2: showMUSIC(); break;
					// case 2: showLEVEL(); break;
					}
				}
			}
		}
	}
	next1:
	// test the mode selected (success)
//	switch (mode) {
//	case 0: show8(0); Delay(); showBASIC(); break;
//	case 1: show8(1); Delay(); showFREE(); break;
//	case 2: show8(2); Delay(); showLEVEL(); break;
//	}
	show8(0b00000000);



	if (mode == 0) {
		// BASIC mode
		// keypad to select the method to generate led
		int method;
		int tempMethod = 0;	// 0: random, 1: keypad
		int methodTable[4][4] = {{0,0,0,-3},{0,0,0,0},{0,0,0,0},{-1,2,1,0}};
		max7219_init(0x00);
		showHELP();
		while (1) {
			Delay();
			for (int col = 0; col < 4; ++col) {
				GPIOB->ODR = (GPIOB->ODR & ~0xF) | (1<<col);
				for (int row = 0; row < 4; ++row) {
					if (GPIOB->IDR & (1<<(4+row))) {
						if (methodTable[row][col] == 2) {
							method = tempMethod;
							show8(0b00000000);
							goto next1_2;
						} else if (methodTable[row][col] == -3) {
							goto next0;
						}
						tempMethod = (tempMethod + methodTable[row][col] + 2) % 2;
						switch (tempMethod) {
						case 0: showHELP(); break;
						case 1: show1245(); break;
						}
					}
				}
			}
		}
		next1_2:

		// 顯示玩家分數
		max7219_init(0xff);
		show8(15);
		int score = 0;
		// showScore(score);

		// 1. 初始化
		unsigned int currentValue = 0;
		int basicTable[4][4] = {{1,2,0,-3},{4,8,0,0},{0,0,0,0},{0,-1,0,0}};
		int arr1[] = {1,3,5,7,9,11,13,15};
		int arr2[] = {2,3,6,7,10,11,14,15};
		int arr4[] = {4,5,6,7,12,13,14,15};
		int arr8[] = {8,9,10,11,12,13,14,15};
		int visited = 0;	// 自由的延續前次亮燈方式
		int previousValue;
		unsigned int seed = TIM2->CNT;
		next1_3:
		if (method == 0) {
			// 隨機亮燈
			seed += 271828192;
			seed *= 314159;
			srand(seed);	/* 設定亂數種子 */
			currentValue = rand() % 16;	// 隨機產生0~15 (0000~1111)
			seed = rand();
		} else if (!visited) {
			// ⽤ keypad 決定
			visited = 1;
			while (1) {
				Delay();
				for (int col = 0; col < 4; ++col) {
					GPIOB->ODR = (GPIOB->ODR & ~0xF) | (1<<col);
					for (int row = 0; row < 4; ++row) {
						if (GPIOB->IDR & (1<<(4+row))) {
							switch (basicTable[row][col]) {
							case 1:
								if (valueInArray(currentValue, arr1, 8)) {
									currentValue -= 1;
								} else currentValue += 1; break;
							case 2:
								if (valueInArray(currentValue, arr2, 8)) {
									currentValue -= 2;
								} else currentValue += 2; break;
							case 4:
								if (valueInArray(currentValue, arr4, 8)) {
									currentValue -= 4;
								} else currentValue += 4; break;
							case 8:
								if (valueInArray(currentValue, arr8, 8)) {
									currentValue -= 8;
								} else currentValue += 8; break;
							case -1:
								previousValue = currentValue;	// 先存起來
								goto next1_4;
							case -3:
								GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
								goto next1;	// 重新讓玩家選 method
							}

							GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
							GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);
						}
					}
				}
			}
		} else {
			currentValue = previousValue;
		}
		next1_4:
		GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);	// led off (4321) 都先關掉
		GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);	// led on (4321)
		// 2. 回合開始
		while (1) {
			// Delay();
			// keypad
			for (int col = 0; col < 4; ++col) {
				GPIOB->ODR = (GPIOB->ODR & ~0xF) | (1<<col);
				for (int row = 0; row < 4; ++row) {
					if (GPIOB->IDR & (1<<(4+row))) {
						switch (basicTable[row][col]) {
						case 1:
							if (valueInArray(currentValue, arr1, 8)) {
								score += 5;   currentValue -= 1;
							} else score = (score < 5) ? 0 : score-5; break;
						case 2:
							if (valueInArray(currentValue, arr2, 8)) {
								score += 5;   currentValue -= 2;
							} else score = (score < 5) ? 0 : score-5; break;
						case 4:
							if (valueInArray(currentValue, arr4, 8)) {
								score += 5;   currentValue -= 4;
							} else score = (score < 5) ? 0 : score-5; break;
						case 8:
							if (valueInArray(currentValue, arr8, 8)) {
								score += 5;   currentValue -= 8;
							} else score = (score < 5) ? 0 : score-5; break;
						case -3:
							GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
							goto next1;	// 重新讓玩家選 method
						}

						GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
						GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);
						show8(15);
						showScore(score);
						Delay();
					}
				}
			}

			// sensor
			if (!(GPIOA->IDR & (1<<6)) || !(GPIOA->IDR & (1<<7)) || !(GPIOA->IDR & (1<<8)) || !(GPIOA->IDR & (1<<9))) {
				if (!(GPIOA->IDR & (1<<6))) {
					if (valueInArray(currentValue, arr1, 8)) {
						score += 5;   currentValue -= 1;
					} else score = (score < 5) ? 0 : score-5;
					Delay();
				}
				if (!(GPIOA->IDR & (1<<7))) {
					if (valueInArray(currentValue, arr2, 8)) {
						score += 5;   currentValue -= 2;
					} else score = (score < 5) ? 0 : score-5;
					Delay();
				}
				if (!(GPIOA->IDR & (1<<8))) {
					if (valueInArray(currentValue, arr4, 8)) {
						score += 5;   currentValue -= 4;
					} else score = (score < 5) ? 0 : score-5;
					Delay();
				}
				if (!(GPIOA->IDR & (1<<9))) {
					if (valueInArray(currentValue, arr8, 8)) {
						score += 5;   currentValue -= 8;
					} else score = (score < 5) ? 0 : score-5;
					Delay();
				}
				GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
				GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);
				show8(15);
				showScore(score);
			}
			

			// show8(15);
			// showScore(score);
			if (currentValue == 0) {
				Delay(); Delay(); Delay(); Delay(); Delay();
				goto next1_3;		// 生成新的回合讓玩家繼續玩（自由的延續前次亮燈方式）
			}
		}
	} else if (mode == 1) {
		// FREE mode
		// keypad to select which game to play
		int game;
		int tempGame = 0;	// 0: 記憶遊戲, 1: 打地鼠, 2: 我是神射手, 3: 刷分仔
		int gameTable[4][4] = {{0,0,0,-3},{0,0,0,0},{0,0,0,0},{-1,2,1,0}};

		next2_1:
		tempGame = 0;
		max7219_init(0x00);
		showGAME(0);
		while (1) {
			Delay();
			for (int col = 0; col < 4; ++col) {
				GPIOB->ODR = (GPIOB->ODR & ~0xF) | (1<<col);
				for (int row = 0; row < 4; ++row) {
					if (GPIOB->IDR & (1<<(4+row))) {
						if (gameTable[row][col] == 2) {
							game = tempGame;
							show8(0b00000000);
							goto next2_2;
						} else if (gameTable[row][col] == -3) {
							goto next0;
						}
						tempGame = (tempGame + gameTable[row][col] + 4) % 4;
						switch (tempGame) {
						case 0: showGAME(0); break;
						case 1: showGAME(1); break;
						case 2: showGAME(2); break;
						case 3: showGAME(3); break;
						}
					}
				}
			}
		}
		next2_2:
		show8(0b00000000);

		if (game == 0) {
			// 記憶遊戲
			int magic[] = {1, 2, 4, 8};
			int memoryTable[4][4] = {{1,2,0,-3},{4,8,0,0},{0,0,0,0},{0,0,0,0}};
			int series_answer[30] = {0};
			int index = 0;					// the current index of the series
			int nextValue;
			unsigned int seed = TIM2->CNT;

			for (int level = 1; level <= 30; ++level) {
				// 隨機產生下一個燈
				seed = TIM2->CNT;
				seed += 271828192;
				seed *= 314159;
				srand(seed);	/* 設定亂數種子 */
				nextValue = magic[rand()%4];
				series_answer[index++] = nextValue;
				// 給提示
				letsHint(series_answer, 30);
				// Delay(); Delay(); Delay();
				// 玩家照順序丟球
				showgogogogo();
				int indexPlaying = 0;
				while (1) {
					// Delay();
					// keypad
					for (int col = 0; col < 4; ++col) {
						GPIOB->ODR = (GPIOB->ODR & ~0xF) | (1<<col);
						for (int row = 0; row < 4; ++row) {
							if (GPIOB->IDR & (1<<(4+row))) {
								switch (memoryTable[row][col]) {
								case 1:
									if (series_answer[indexPlaying++] == 1) {
										// do something
										GPIOC->ODR = GPIOC->ODR & ~(1 << 3);
										// TODO: 調快速度
										Delay(); Delay();
										// Delay(); Delay(); Delay(); Delay(); Delay();
									} else goto bye; break;
								case 2:
									if (series_answer[indexPlaying++] == 2) {
										// do something
										GPIOC->ODR = GPIOC->ODR & ~(2 << 3);
										Delay(); Delay();
										// Delay(); Delay(); Delay(); Delay(); Delay();
									} else goto bye; break;
								case 4:
									if (series_answer[indexPlaying++] == 4) {
										// do something
										GPIOC->ODR = GPIOC->ODR & ~(4 << 3);
										Delay(); Delay();
										// Delay(); Delay(); Delay(); Delay(); Delay();
									} else goto bye; break;
								case 8:
									if (series_answer[indexPlaying++] == 8) {
										// do something
										GPIOC->ODR = GPIOC->ODR & ~(8 << 3);
										Delay(); Delay();
										// Delay(); Delay(); Delay(); Delay(); Delay();
									} else goto bye; break;
								case -3:
									GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
									goto next2_1;	// 重新讓玩家選 game
								}

								GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
								Delay();
							}
						}
					}

					// sensor
					if (!(GPIOA->IDR & (1<<6)) || !(GPIOA->IDR & (1<<7)) || !(GPIOA->IDR & (1<<8)) || !(GPIOA->IDR & (1<<9))) {
						if (!(GPIOA->IDR & (1<<6))) {
							GPIOC->ODR = GPIOC->ODR & ~(1 << 3);
							if (series_answer[indexPlaying] == 1) {
								// do something
								indexPlaying++;
								Delay(); Delay();
								// Delay(); Delay(); Delay(); Delay(); Delay();
							} else goto bye;
						}
						if (!(GPIOA->IDR & (1<<7))) {
							GPIOC->ODR = GPIOC->ODR & ~(2 << 3);
							if (series_answer[indexPlaying] == 2) {
								// do something
								indexPlaying++;
								Delay(); Delay();
								// Delay(); Delay(); Delay(); Delay(); Delay();
							} else goto bye;
						}
						if (!(GPIOA->IDR & (1<<8))) {
							GPIOC->ODR = GPIOC->ODR & ~(4 << 3);
							if (series_answer[indexPlaying] == 4) {
								// do something
								indexPlaying++;
								Delay(); Delay();
								// Delay(); Delay(); Delay(); Delay(); Delay();
							} else goto bye;
						}
						if (!(GPIOA->IDR & (1<<9))) {
							GPIOC->ODR = GPIOC->ODR & ~(8 << 3);
							if (series_answer[indexPlaying] == 8) {
								// do something
								indexPlaying++;
								Delay(); Delay();
								// Delay(); Delay(); Delay(); Delay(); Delay();
							} else goto bye;
						}
						GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
						Delay();
					}

					if (indexPlaying == index) {
						break;
					}
				}

				seed = rand();
				showGoodJob();
				Delay(); Delay(); Delay(); Delay(); Delay();
				Delay(); Delay(); Delay(); Delay(); Delay();
				Delay();
				show8(0b00000000);
			}

			bye:
			showSEEYOU();
			Delay(); Delay(); Delay(); Delay(); Delay();
			Delay(); Delay(); Delay(); Delay(); Delay();
			GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
			Delay(); Delay(); Delay(); Delay(); Delay();
			Delay(); Delay(); Delay(); Delay(); Delay();
			show8(0b00000000);
			goto next2_1;
		} else if (game == 1) {
			// 打地鼠
			// keypad to select the method to choose the gaming speed
			int speed;
			int tempSpeed = 0;	// 0 -> 9,	越來越快
			int speedTable[4][4] = {{0,0,0,-3},{0,0,0,0},{0,0,0,0},{-1,2,1,0}};
			max7219_init(0x00);
			showSPEED(0);
			while (1) {
				Delay();
				for (int col = 0; col < 4; ++col) {
					GPIOB->ODR = (GPIOB->ODR & ~0xF) | (1<<col);
					for (int row = 0; row < 4; ++row) {
						if (GPIOB->IDR & (1<<(4+row))) {
							if (speedTable[row][col] == 2) {
								speed = tempSpeed;
								show8(0b00000000);
								goto next5_1;
							} else if (speedTable[row][col] == -3) {
								goto next2_1;
							}
							tempSpeed = (tempSpeed + speedTable[row][col] + 10) % 10;
							switch (tempSpeed) {
							case 0: showSPEED(0); break;
							case 1: showSPEED(1); break;
							case 2: showSPEED(2); break;
							case 3: showSPEED(3); break;
							case 4: showSPEED(4); break;
							case 5: showSPEED(5); break;
							case 6: showSPEED(6); break;
							case 7: showSPEED(7); break;
							case 8: showSPEED(8); break;
							case 9: showSPEED(9); break;
							}
						}
					}
				}
			}

			next5_1:
			// 顯示玩家分數
			max7219_init(0xff);
			show8(15);
			int score = 0;
			// showScore(score);

			// 1. 初始化
			unsigned int currentValue = 0;
			int basicTable[4][4] = {{1,2,0,-3},{4,8,0,0},{0,0,0,0},{0,-1,0,0}};
			int arr1[] = {1,3,5,7,9,11,13,15};
			int arr2[] = {2,3,6,7,10,11,14,15};
			int arr4[] = {4,5,6,7,12,13,14,15};
			int arr8[] = {8,9,10,11,12,13,14,15};
			unsigned int seed = TIM2->CNT;

			while (1) {
				// 隨機亮燈
				seed += 271828192;
				seed *= 314159;
				srand(seed);	/* 設定亂數種子 */
				currentValue = rand() % 16;	// 隨機產生0~15 (0000~1111)
				seed = rand();
				// 秀出地鼠
				GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
				GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);
				// 調整速度
				int timeToHide;		// 過多久地鼠們會躲起來
				switch(speed) {		// 單位是幾秒 // TODO: 待調整
				case 0: timeToHide = 60; break;
				case 1: timeToHide = 55; break;
				case 2: timeToHide = 50; break;
				case 3: timeToHide = 45; break;
				case 4: timeToHide = 40; break;
				case 5: timeToHide = 35; break;
				case 6: timeToHide = 30; break;
				case 7: timeToHide = 25; break;
				case 8: timeToHide = 20; break;
				case 9: timeToHide = 10; break;
				}
				timeToHide *= 5000;	// 每 5000 約 1 秒
				// 2. 回合開始
				while (1) {
					timeToHide--;
					// Delay();
					// keypad
					for (int col = 0; col < 4; ++col) {
						GPIOB->ODR = (GPIOB->ODR & ~0xF) | (1<<col);
						for (int row = 0; row < 4; ++row) {
							if (GPIOB->IDR & (1<<(4+row))) {
								switch (basicTable[row][col]) {
								case 1:
									if (valueInArray(currentValue, arr1, 8)) {
										score += 5;   currentValue -= 1;
									} else score = (score < 5) ? 0 : score-5; break;
								case 2:
									if (valueInArray(currentValue, arr2, 8)) {
										score += 5;   currentValue -= 2;
									} else score = (score < 5) ? 0 : score-5; break;
								case 4:
									if (valueInArray(currentValue, arr4, 8)) {
										score += 5;   currentValue -= 4;
									} else score = (score < 5) ? 0 : score-5; break;
								case 8:
									if (valueInArray(currentValue, arr8, 8)) {
										score += 5;   currentValue -= 8;
									} else score = (score < 5) ? 0 : score-5; break;
								case -3:
									GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
									goto next1;	// 重新讓玩家選 method
								}

								GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
								GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);
								show8(15);
								showScore(score);
								Delay();
							}
						}
					}

					// sensor
					if (!(GPIOA->IDR & (1<<6)) || !(GPIOA->IDR & (1<<7)) || !(GPIOA->IDR & (1<<8)) || !(GPIOA->IDR & (1<<9))) {
						if (!(GPIOA->IDR & (1<<6))) {
							if (valueInArray(currentValue, arr1, 8)) {
								score += 5;   currentValue -= 1;
							} else score = (score < 5) ? 0 : score-5;
							Delay();
						}
						if (!(GPIOA->IDR & (1<<7))) {
							if (valueInArray(currentValue, arr2, 8)) {
								score += 5;   currentValue -= 2;
							} else score = (score < 5) ? 0 : score-5;
							Delay();
						}
						if (!(GPIOA->IDR & (1<<8))) {
							if (valueInArray(currentValue, arr4, 8)) {
								score += 5;   currentValue -= 4;
							} else score = (score < 5) ? 0 : score-5;
							Delay();
						}
						if (!(GPIOA->IDR & (1<<9))) {
							if (valueInArray(currentValue, arr8, 8)) {
								score += 5;   currentValue -= 8;
							} else score = (score < 5) ? 0 : score-5;
							Delay();
						}
						GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
						GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);
						show8(15);
						showScore(score);
					}

					if (timeToHide <= 25000 && timeToHide % 5000 == 0) {	// 結束前 5 秒會閃爍 5 下
						GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
						Delay();
						GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);
					}

					if (timeToHide == 0) {
						if (currentValue != 0) {	// 代表沒完成
							score = (score <= 10) ? 0 : score - 10;
						}
						GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
						Delay();
						GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);
						showScore(score);
						break;
					}

					if (currentValue == 0) {
						Delay(); Delay(); Delay(); Delay(); Delay();
						break;
					}
				}
			}
		} else if (game == 2) {
			// 我是神射⼿
			// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝ 關 卡 參 數 ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
			// TODO: 多試驗幾次來調出最佳比例！
			// TODO: 確定 ballLeft 的定義
			int ballMax[] = {9, 9, 8, 8, 7, 7, 6, 6, 5, 4};		// 每關球數
			int theGoal[] = {		// 每關的目標杯子 5421
				0b0100, 0b1100, 0b0100, 0b1100, 0b0001,
				0b0011, 0b1100, 0b0011, 0b1110, 0b1111
			};
			// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
			int basicTable[4][4] = {{1,2,0,-3},{4,8,0,0},{0,0,0,0},{0,-1,0,0}};
			int arr1[] = {1,3,5,7,9,11,13,15};
			int arr2[] = {2,3,6,7,10,11,14,15};
			int arr4[] = {4,5,6,7,12,13,14,15};
			int arr8[] = {8,9,10,11,12,13,14,15};
			// 進到關卡
			for (int stage = 0; stage <= 9; ++stage) {
				int ballLeft = ballMax[stage];
				int currentValue = theGoal[stage];
				showBALL(stage, ballLeft);
				// 亮提示燈
				GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
				GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);
				// 開始玩吧！
				while (1) {
					// Delay();
					// keypad
					for (int col = 0; col < 4; ++col) {
						GPIOB->ODR = (GPIOB->ODR & ~0xF) | (1<<col);
						for (int row = 0; row < 4; ++row) {
							if (GPIOB->IDR & (1<<(4+row))) {
								switch (basicTable[row][col]) {
								case 1:
									if (valueInArray(currentValue, arr1, 8)) {
										ballLeft -= 1;   currentValue -= 1;
									} else ballLeft -= 1; break;
								case 2:
									if (valueInArray(currentValue, arr2, 8)) {
										ballLeft -= 1;   currentValue -= 2;
									} else ballLeft -= 1; break;
								case 4:
									if (valueInArray(currentValue, arr4, 8)) {
										ballLeft -= 1;   currentValue -= 4;
									} else ballLeft -= 1; break;
								case 8:
									if (valueInArray(currentValue, arr8, 8)) {
										ballLeft -= 1;   currentValue -= 8;
									} else ballLeft -= 1; break;
								case -3:
									GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
									goto next2_1;	// 重新讓玩家選 game
								}

								GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
								GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);
								showBALL(stage, ballLeft);
								Delay();
							}
						}
					}

					// sensor
					if (!(GPIOA->IDR & (1<<6)) || !(GPIOA->IDR & (1<<7)) || !(GPIOA->IDR & (1<<8)) || !(GPIOA->IDR & (1<<9))) {
						if (!(GPIOA->IDR & (1<<6))) {
							if (valueInArray(currentValue, arr1, 8)) {
								ballLeft -= 1;   currentValue -= 1;
							} else ballLeft -= 1;
							Delay();
						}
						if (!(GPIOA->IDR & (1<<7))) {
							if (valueInArray(currentValue, arr2, 8)) {
								ballLeft -= 1;   currentValue -= 2;
							} else ballLeft -= 1;
							Delay();
						}
						if (!(GPIOA->IDR & (1<<8))) {
							if (valueInArray(currentValue, arr4, 8)) {
								ballLeft -= 1;   currentValue -= 4;
							} else ballLeft -= 1;
							Delay();
						}
						if (!(GPIOA->IDR & (1<<9))) {
							if (valueInArray(currentValue, arr8, 8)) {
								ballLeft -= 1;   currentValue -= 8;
							} else ballLeft -= 1;
							Delay();
						}
						GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
						GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);
						showBALL(stage, ballLeft);
					}

					if (currentValue == 0) {
						Delay(); Delay(); Delay(); Delay(); Delay();
						break;		// 進到下一關
					}

					if (ballLeft == 0) {
						goto bye;
					}
				}
			}
			showGoodJob();
			Delay(); Delay(); Delay(); Delay(); Delay();
			Delay(); Delay(); Delay(); Delay(); Delay();
			Delay();
			Delay(); Delay(); Delay(); Delay(); Delay();
			Delay(); Delay(); Delay(); Delay(); Delay();
			Delay();
			show8(0b00000000);
			goto next2_1;	// 重新讓玩家選 game
		} else if (game == 3) {
			// 刷分仔
			int basicTable[4][4] = {{1,2,0,-3},{4,8,0,0},{0,0,0,0},{0,-1,0,0}};
			int arr1[] = {1,3,5,7,9,11,13,15};
			int arr2[] = {2,3,6,7,10,11,14,15};
			int arr4[] = {4,5,6,7,12,13,14,15};
			int arr8[] = {8,9,10,11,12,13,14,15};
			int score = 0;
			// 進到關卡
			for (int stage = 1; stage <= 5; ++stage) {
				int timeLeft = 20;
				timeLeft++;
				int temp = timeLeft * 1000000 / 9;
				// 亮提示燈
				unsigned int seed = TIM2->CNT;
				seed += 271828192;
				seed *= 314159;
				srand(seed);	/* 設定亂數種子 */
				int currentValue = rand() % 15 + 1;	// 隨機產生1~15 (0001~1111)
				seed = rand();
				GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
				GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);
				// 	int ballLeft = ballMax[stage];
				// int currentValue = theGoal[stage];
				// 	showBALL(stage, ballLeft);
			// 	// 開始玩吧！
				while (temp--) {
					if (temp % 10000 == 0) {
						showTimeLeft(stage, temp * 9 / 1000000);
						showScore(score);
						timeLeft--;
					}
					
					// if (temp % 1000000 == 0) {
					// 	// 亮提示燈
					// 	unsigned int seed = TIM2->CNT;
					// 	seed += 271828192;
					// 	seed *= 314159;
					// 	srand(seed);	/* 設定亂數種子 */
					// 	int currentValue = rand() % 15 + 1;	// 隨機產生1~15 (0001~1111)
					// 	seed = rand();
					// 	GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
					// 	GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);
					// }

					// sensor
					if (!(GPIOA->IDR & (1<<6)) || !(GPIOA->IDR & (1<<7)) || !(GPIOA->IDR & (1<<8)) || !(GPIOA->IDR & (1<<9))) {
						if (!(GPIOA->IDR & (1<<6))) {
							if (valueInArray(currentValue, arr1, 8)) {
								if (temp <= 700000) score += 3;
								else score += 2;
							} else score -= 0;
							Delay();
						}
						if (!(GPIOA->IDR & (1<<7))) {
							if (valueInArray(currentValue, arr2, 8)) {
								if (temp <= 700000) score += 3;
								else score += 2;
							} else score -= 0;
							Delay();
						}
						if (!(GPIOA->IDR & (1<<8))) {
							if (valueInArray(currentValue, arr4, 8)) {
								if (temp <= 700000) score += 3;
								else score += 2;
							} else score -= 0;
							Delay();
						}
						if (!(GPIOA->IDR & (1<<9))) {
							if (valueInArray(currentValue, arr8, 8)) {
								if (temp <= 700000) score += 3;
								else score += 2;
							} else score -= 0;
							Delay();
						}
						// GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
						// GPIOC->ODR = GPIOC->ODR & ~(currentValue << 3);
						// showBALL(stage, ballLeft);
					}

			// 		if (currentValue == 0) {
			// 			Delay(); Delay(); Delay(); Delay(); Delay();
			// 			break;		// 進到下一關
			// 		}

			// 		if (ballLeft == 0) {
			// 			goto bye;
			// 		}
				}
				switch (stage) {
					case 1: if (score < 50) { goto bye2; } break;
					case 2: if (score < 150) { goto bye2; } break;
					case 3: if (score < 250) { goto bye2; } break;
					case 4: if (score < 350) { goto bye2; } break;
					case 5: if (score < 450) { goto bye2; } break;
				}
			}
			bye2:
			max7219_init(0x00);
			showSEEYOU();
			Delay(); Delay(); Delay(); Delay(); Delay();
			Delay(); Delay(); Delay(); Delay(); Delay();
			GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
			Delay(); Delay(); Delay(); Delay(); Delay();
			Delay(); Delay(); Delay(); Delay(); Delay();
			show8(0b00000000);
			goto next2_1;
		}
	} else if (mode == 2) {
		// MUSIC mode
		PWM_channel_init(); //TODO:
		showLIVE();
		Delay();
		giveMeBeep(A4, 1);
		int musicTable[4][4] = {{1,2,0,-3},{4,8,0,0},{0,0,0,55},{-6,11,+6,88}};
		// int note1[] = {C3,G3,C4,G4,C5,G5};
		// int note2[] = {D3,A3,D4,A4,D5,A5};
		// int note3[] = {E3,B3,E4,B4,E5,B5};
		// int note4[] = {F3,C4,F4,C5,F5,C6};
		// int note[5][6] = {{1,1,1,1,1,1},{C3,G3,C4,G4,C5,G5},{D3,A3,D4,A4,D5,A5},{E3,B3,E4,B4,E5,B5},{F3,C4,F4,C5,F5,C6}};
		int note[5][6] = {{1,1,1,1,1,1},{C3,G3,C4,G4,C5,G5},{D3,A3,D4,A4,D5,A5},{E3,B3,E4,B4,E5,B5},{F3,1,F4,1,F5,1}};
		int cur = 2;
		int isRecord = 0;
		int series_cup[100] = {0};
		int series_pos[100] = {0};
		int head = 0;		// index of the recording head
		while (1) {
			// Delay();
			// keypad
			for (int col = 0; col < 4; ++col) {
				GPIOB->ODR = (GPIOB->ODR & ~0xF) | (1<<col);
				for (int row = 0; row < 4; ++row) {
					if (GPIOB->IDR & (1<<(4+row))) {
						switch (musicTable[row][col]) {
						case 11:	// 開啟／關閉記錄聲音
							if (isRecord) {
								showLIVE();
							} else {
								showREC();
							}
							isRecord = !isRecord;
							break;
						case 55:	// 播放紀錄
							for (int i = 0; i < head; ++i) {
								giveMeBeep(note[series_cup[i]][series_pos[i]], 1);
							}
							break;
						case 88:	// 刪掉紀錄
							setArray(series_cup, 100, 0);
							setArray(series_pos, 100, 0);
							head = 0;
							break;
						case 1:
							GPIOC->ODR = GPIOC->ODR & ~(1 << 3);
							giveMeBeep(note[1][cur], 1);
							Delay(); Delay();
							if (isRecord) {
								series_cup[head] = 1;
								series_pos[head++] = cur;
							}
							break;
						case 2:
							GPIOC->ODR = GPIOC->ODR & ~(2 << 3);
							giveMeBeep(note[2][cur], 1);
							Delay(); Delay();
							if (isRecord) {
								series_cup[head] = 2;
								series_pos[head++] = cur;
							}
							break;
						case 4:
							GPIOC->ODR = GPIOC->ODR & ~(4 << 3);
							giveMeBeep(note[3][cur], 1);
							Delay(); Delay();
							if (isRecord) {
								series_cup[head] = 3;
								series_pos[head++] = cur;
							}
							break;
						case 8:
							GPIOC->ODR = GPIOC->ODR & ~(8 << 3);
							giveMeBeep(note[4][cur], 1);
							Delay(); Delay();
							if (isRecord) {
								series_cup[head] = 4;
								series_pos[head++] = cur;
							}
							break;
						case -6:
							cur = cur == 0 ? 0 : cur - 1;
							// TODO: 新加上去的
							Delay();
							break;
						case +6:
							cur = cur == 5 ? 5 : cur + 1;
							Delay();
							break;
						case -3:
							GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
							goto next0;	// 重新讓玩家選 mode
						}

						GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
						Delay();
					}
				}
			}

			// sensor
			if (!(GPIOA->IDR & (1<<6)) || !(GPIOA->IDR & (1<<7)) || !(GPIOA->IDR & (1<<8)) || !(GPIOA->IDR & (1<<9))) {
				if (!(GPIOA->IDR & (1<<6))) {
					GPIOC->ODR = GPIOC->ODR & ~(1 << 3);
					giveMeBeep(note[1][cur], 1);
					Delay(); Delay();
					if (isRecord) {
						series_cup[head] = 1;
						series_pos[head++] = cur;
					}
				}
				if (!(GPIOA->IDR & (1<<7))) {
					GPIOC->ODR = GPIOC->ODR & ~(2 << 3);
					giveMeBeep(note[2][cur], 1);
					Delay(); Delay();
					if (isRecord) {
						series_cup[head] = 2;
						series_pos[head++] = cur;
					}
				}
				if (!(GPIOA->IDR & (1<<8))) {
					GPIOC->ODR = GPIOC->ODR & ~(4 << 3);
					giveMeBeep(note[3][cur], 1);
					Delay(); Delay();
					if (isRecord) {
						series_cup[head] = 3;
						series_pos[head++] = cur;
					}
				}
				if (!(GPIOA->IDR & (1<<9))) {
					GPIOC->ODR = GPIOC->ODR & ~(8 << 3);
					giveMeBeep(note[4][cur], 1);
					Delay(); Delay();
					if (isRecord) {
						series_cup[head] = 4;
						series_pos[head++] = cur;
					}
				}
				GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);
				Delay();
			}
		}
		// // LEVEL mode
		// show8(0b00000001);	// --------
		// Delay(); Delay(); Delay(); Delay(); Delay();
		// Delay(); Delay(); Delay(); Delay(); Delay();
		// Delay();
		// Delay(); Delay(); Delay(); Delay(); Delay();
		// Delay(); Delay(); Delay(); Delay(); Delay();
		// Delay();
		// show8(0b00000000);
		// goto next0;
	}




	// keypad for testing
//	int Table[4][4] = {{1,2,3,0},{4,5,6,0},{7,8,9,0},{0,0,0,0}};
//	while (1) {
//		//Delay();
//		for (int col = 0; col < 4; ++col) {
//			GPIOB->ODR = (GPIOB->ODR & ~0xF) | (1<<col);
//			for (int row = 0; row < 4; ++row) {
//				if (GPIOB->IDR & (1<<(4+row))) {
//					 show8(Table[row][col]);
//					 if (Table[row][col] % 2 == 0)
////					     GPIOC->BSRR = set_1(GPIOC->BSRR,led,0);
//						 GPIOC->ODR = GPIOC->ODR & ~(0b1111 << 3);		// 0: led on
////						 GPIOC->ODR = GPIOC->ODR & 0xff87;		// 0: led on
//					 else
////						 GPIOC->BRR = set_1(GPIOC->BRR,led,0);
//						 GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);		// 1: led off
////						 GPIOC->ODR = GPIOC->ODR | 0x0078;		// 1: led off
//				}
//			}
//		}
//	}

	return 0;
}



void GPIO_init()
{
	RCC->AHB2ENR = RCC->AHB2ENR | 0x7;	// Enable GPIOA,B,C

	// Set keypad_output
	GPIOB->MODER = set(GPIOB->MODER, keypad_output, 4, 1);
	GPIOB->OSPEEDR = set(GPIOB->OSPEEDR, keypad_output, 4, 1);
	GPIOB->ODR = set_1(GPIOB->ODR, keypad_output, 4);

	// Set keypad_input
	GPIOB->MODER = set(GPIOB->MODER, keypad_input, 4, 0);
	GPIOB->PUPDR = set(GPIOB->PUPDR, keypad_input, 4, 2);

	// Set max7219
	GPIOC->MODER = set(GPIOC->MODER, max7219, 3, 1);
	GPIOC->OSPEEDR = set(GPIOC->OSPEEDR, max7219, 3, 2);

	// Set button
	GPIOC->MODER = set(GPIOC->MODER, button, 1, 0);
	GPIOC->PUPDR = set(GPIOC->PUPDR, button, 1, 1);

	// Set led
	GPIOC->MODER = set(GPIOC->MODER, led, 4, 1);

	// Set sensor
	GPIOA->MODER = set(GPIOA->MODER, sensor, 4, 0);
	GPIOA->PUPDR = set(GPIOA->PUPDR, sensor, 4, 2);

	// Set beep
	GPIOA->AFR[0] = ((GPIOA->AFR[0]) & 0xFF0FFFFF) | 0x100000; //AF2 on PA5(Refer to TIM
	GPIOA->MODER = set(GPIOA->MODER, beep, 1, 2); //Set to alternative function
}

void Delay()
{
	for (int i = 0; i < 300; ++i) {
		for (int j = 0; j < 300; ++j) {
			// do nothing
		}
	}
}

void show8(int number)
{
	for (int i = 1; i <= 8; ++i) {
		MAX7219Send(i, number);
	}
}

void showScore(int score)
{
	int i = 1;
	do {
		MAX7219Send(i++, score % 10);
	} while (score /= 10);
}

void showTimeLeft(int stage, int timeLeft)
{
	max7219_init(0xff);
	show8(15);
	MAX7219Send(8, stage);
	int i = 5;
	do {
		MAX7219Send(i++, timeLeft % 10);
	} while (timeLeft /= 10);
	// max7219_init(0x00);
}

// void NVIC_config()
// {
// 	// set Interrupt Set Enable Register( enable exti 9_5 interrupt)
// 	NVIC->ISER[0] = set(NVIC->ISER[0], sensor_interrupt, 1, 1);
// }

// void EXTI_config()
// {
// 	// SYSCFG and exti enable
// 	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
// 	SYSCFG->EXTICR[1] |= 0x1110;
// 	SYSCFG->EXTICR[2] |= 0x1;

// 	// set interrupt mask for keypad_input
// 	EXTI->IMR1 = set(EXTI->IMR1, sensor, 4, 1);

// 	// set raising edge interrupt
// 	EXTI->RTSR1 = set(EXTI->RTSR1, sensor, 4, 1);

// 	// reset pending exti to zero
// 	EXTI->PR1 &= set(EXTI->PR1, sensor, 4, 1);
// }

void Timer_init()
{
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;	// timer2 clock enable
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // timer2 clock enable (beep)
	TIM2->CR1 &= ~TIM_CR1_DIR;	// up counter
	TIM2->PSC = 4000;	// Prescaler
	TIM2->ARR = 1000;	// Reload value
	// beep
	// TIM2->PSC = 51;	// Prescaler
	// TIM2->ARR = 100;	// Reload value
	TIM2->EGR |= TIM_EGR_UG;	// Reinitialize the counter. CNT takes the auto-reload value
	TIM2->CCER = (TIM2->CCER) & 0;
	TIM2->CR1 |= TIM_CR1_CEN;	// Start timer
	// TIM2->CR1 &= ~TIM_CR1_CEN; // stop timer
}

void PWM_channel_init()
{
	TIM2->CCER = (TIM2->CCER) | 1;

	// Setting TIM2 Ch1 as output
	TIM2->CCMR1 = (TIM2->CCMR1) & 0xFFFFFFFC;

	// Setting TIM2 Ch1 as PWM mode2
	TIM2->CCMR1 = ((TIM2->CCMR1) & 0xFFFFFF8F) | 0x70;
	TIM2->CCR1 = 50;
}

void delay(int msec)
{
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
	while (TIM2->CNT < msec)
	{

	}
	TIM2->CR1 &= ~TIM_CR1_CEN;
}

void giveMeBeep(int frequency, int duration)
{
	// beep
	TIM2->PSC = 2000000/100/frequency;	// Prescaler
	TIM2->ARR = 100;	// Reload value
	TIM2->CR1 |= TIM_CR1_CEN;	// Start timer
	// int i = 5 * duration;
	int i = 1 * duration;
	while (i--)
		Delay();
	TIM2->CR1 &= ~TIM_CR1_CEN; // stop timer
	TIM2->EGR |= TIM_EGR_UG; // reset timer
}

// void EXTI9_5_IRQHandler()
// {
// 	// test for the sensor
// 	// the sensor is active low!!!
// 	// https://www.playembedded.org/blog/detecting-obstacle-with-ir-sensor-and-arduino/
// 	if (!(GPIOA->IDR & (1<<9))) {
// 		GPIOC->ODR = GPIOC->ODR & ~(15 << 3);
// 	} else {
// 		GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);	// 燈先關掉
// 	}
	
// 	// reset pending exti to zero
// 	EXTI->PR1 &= set(EXTI->PR1, sensor, 4, 1);
// 	// clear nvic pending
// 	NVIC->ICPR[0] = set(NVIC->ICPR[0], sensor_interrupt, 1, 1);
// }

void showBASIC()
{
	MAX7219Send(8, 0b00000000);		//
	MAX7219Send(7, 0b00000000);		//
	MAX7219Send(6, 0b00000000);		//
	MAX7219Send(5, 0b01111111);		// B
	MAX7219Send(4, 0b01110111);		// A
	MAX7219Send(3, 0b01011011);		// S
	MAX7219Send(2, 0b00110000);		// I
	MAX7219Send(1, 0b01001110);		// C
}

void showFREE()
{
	MAX7219Send(8, 0b00000000);		//
	MAX7219Send(7, 0b00000000);		//
	MAX7219Send(6, 0b00000000);		//
	MAX7219Send(5, 0b00000000);		//
	MAX7219Send(4, 0b01000111);		// F
	MAX7219Send(3, 0b01110111);		// R
	MAX7219Send(2, 0b01001111);		// E
	MAX7219Send(1, 0b01001111);		// E
}

void showMUSIC()
{
	MAX7219Send(8, 0b00000000);		//
	MAX7219Send(7, 0b00000000);		//
	MAX7219Send(6, 0b00000000);		//
	MAX7219Send(5, 0b01110110);		// M
	MAX7219Send(4, 0b00111110);		// U
	MAX7219Send(3, 0b01011011);		// S
	MAX7219Send(2, 0b00110000);		// I
	MAX7219Send(1, 0b01001110);		// C
}

// void showLEVEL()
// {
// 	MAX7219Send(8, 0b00000000);		//
// 	MAX7219Send(7, 0b00000000);		//
// 	MAX7219Send(6, 0b00000000);		//
// 	MAX7219Send(5, 0b00001110);		// L
// 	MAX7219Send(4, 0b01001111);		// E
// 	MAX7219Send(3, 0b00111110);		// V
// 	MAX7219Send(2, 0b01001111);		// E
// 	MAX7219Send(1, 0b00001110);		// L
// }

void showHELP()
{
	MAX7219Send(8, 0b00000000);		//
	MAX7219Send(7, 0b00000000);		//
	MAX7219Send(6, 0b00000000);		//
	MAX7219Send(5, 0b00000000);		//
	MAX7219Send(4, 0b00110111);		// H
	MAX7219Send(3, 0b01001111);		// E
	MAX7219Send(2, 0b00001110);		// L
	MAX7219Send(1, 0b01100111);		// P
}

void show1245()
{
	MAX7219Send(8, 0b00000000);		//
	MAX7219Send(7, 0b00000000);		//
	MAX7219Send(6, 0b00000000);		//
	MAX7219Send(5, 0b00000000);		//
	MAX7219Send(4, 0b00110000);		// 1
	MAX7219Send(3, 0b01101101);		// 2
	MAX7219Send(2, 0b00110011);		// 4
	MAX7219Send(1, 0b01011011);		// 5
}

void showGAME(int i)
{
	MAX7219Send(8, 0b01011111);		// G
	MAX7219Send(7, 0b01110111);		// A
	MAX7219Send(6, 0b01110110);		// M
	MAX7219Send(5, 0b01001111);		// E
	MAX7219Send(4, 0b00000000);		//
	MAX7219Send(3, 0b00000000);		//
	MAX7219Send(2, 0b00000000);		//
	switch (i) {
	case 0: MAX7219Send(1, 0b01111110); break;
	case 1: MAX7219Send(1, 0b00110000); break;
	case 2: MAX7219Send(1, 0b01101101); break;
	case 3: MAX7219Send(1, 0b01111001); break;
	case 4: MAX7219Send(1, 0b00110011); break;
	case 5: MAX7219Send(1, 0b01011011); break;
	}
}

void letsHint(int arr[], int n)
{
	for (int i = 0; i < n; ++i) {
		if (arr[i] == 0)
			break;
		GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);	// 關燈
		GPIOC->ODR = GPIOC->ODR & ~(arr[i] << 3);
		// TODO: 速度調快
		Delay(); Delay();
		// Delay(); Delay(); Delay(); Delay(); Delay();
		GPIOC->ODR = GPIOC->ODR | (0b1111 << 3);	// 關燈
		Delay(); Delay();
		// Delay(); Delay(); Delay(); Delay(); Delay();
	}
}

void showgogogogo()
{
	MAX7219Send(8, 0b01111011);		// g
	MAX7219Send(7, 0b01100011);		// o
	MAX7219Send(6, 0b01111011);		// g
	MAX7219Send(5, 0b01100011);		// o
	MAX7219Send(4, 0b01111011);		// g
	MAX7219Send(3, 0b01100011);		// o
	MAX7219Send(2, 0b01111011);		// g
	MAX7219Send(1, 0b01100011);		// o
}

void showGoodJob()
{
	MAX7219Send(8, 0b01011111);		// G
	MAX7219Send(7, 0b00011101);		// o
	MAX7219Send(6, 0b00011101);		// o
	MAX7219Send(5, 0b00111101);		// d
	MAX7219Send(4, 0b00000000);		//
	MAX7219Send(3, 0b00111100);		// J
	MAX7219Send(2, 0b00011101);		// o
	MAX7219Send(1, 0b00011111);		// b
}

void showSEEYOU()
{
	MAX7219Send(8, 0b00000000);		//
	MAX7219Send(7, 0b01011011);		// S
	MAX7219Send(6, 0b01001111);		// E
	MAX7219Send(5, 0b01001111);		// E
	MAX7219Send(4, 0b00000000);		//
	MAX7219Send(3, 0b00111011);		// Y
	MAX7219Send(2, 0b01111110);		// O
	MAX7219Send(1, 0b00111110);		// U
}

void showSPEED(int i)
{
	MAX7219Send(8, 0b01011011);		// S
	MAX7219Send(7, 0b01100111);		// P
	MAX7219Send(6, 0b01001111);		// E
	MAX7219Send(5, 0b01001111);		// E
	MAX7219Send(4, 0b00111101);		// d
	MAX7219Send(3, 0b00000000);		// 
	MAX7219Send(2, 0b00000000);		// 
	switch (i) {
	case 0: MAX7219Send(1, 0b01111110); break;
	case 1: MAX7219Send(1, 0b00110000); break;
	case 2: MAX7219Send(1, 0b01101101); break;
	case 3: MAX7219Send(1, 0b01111001); break;
	case 4: MAX7219Send(1, 0b00110011); break;
	case 5: MAX7219Send(1, 0b01011011); break;
	case 6: MAX7219Send(1, 0b01011111); break;
	case 7: MAX7219Send(1, 0b01110000); break;
	case 8: MAX7219Send(1, 0b01111111); break;
	case 9: MAX7219Send(1, 0b01111011); break;
	}
}

void showBALL(int stage, int ballLeft)
{
	switch (stage) {
	case 0: MAX7219Send(8, 0b01111110); break;
	case 1: MAX7219Send(8, 0b00110000); break;
	case 2: MAX7219Send(8, 0b01101101); break;
	case 3: MAX7219Send(8, 0b01111001); break;
	case 4: MAX7219Send(8, 0b00110011); break;
	case 5: MAX7219Send(8, 0b01011011); break;
	case 6: MAX7219Send(8, 0b01011111); break;
	case 7: MAX7219Send(8, 0b01110000); break;
	case 8: MAX7219Send(8, 0b01111111); break;
	case 9: MAX7219Send(8, 0b01111011); break;
	}
	MAX7219Send(7, 0b00000000);		// 
	MAX7219Send(6, 0b01111111);		// B
	MAX7219Send(5, 0b01110111);		// A
	MAX7219Send(4, 0b00001110);		// L
	MAX7219Send(3, 0b00001110);		// L
	MAX7219Send(2, 0b00000000);		// 
	switch (ballLeft) {
	case 0: MAX7219Send(1, 0b01111110); break;
	case 1: MAX7219Send(1, 0b00110000); break;
	case 2: MAX7219Send(1, 0b01101101); break;
	case 3: MAX7219Send(1, 0b01111001); break;
	case 4: MAX7219Send(1, 0b00110011); break;
	case 5: MAX7219Send(1, 0b01011011); break;
	case 6: MAX7219Send(1, 0b01011111); break;
	case 7: MAX7219Send(1, 0b01110000); break;
	case 8: MAX7219Send(1, 0b01111111); break;
	case 9: MAX7219Send(1, 0b01111011); break;
	}
}

void showLIVE()
{
	MAX7219Send(8, 0b00000000);		//
	MAX7219Send(7, 0b00000000);		//
	MAX7219Send(6, 0b00000000);		//
	MAX7219Send(5, 0b00000000);		//
	MAX7219Send(4, 0b00001110);		// L
	MAX7219Send(3, 0b00110000);		// I
	MAX7219Send(2, 0b00111110);		// V
	MAX7219Send(1, 0b01001111);		// E
}

void showREC()
{
	MAX7219Send(8, 0b00000000);		//
	MAX7219Send(7, 0b00000000);		//
	MAX7219Send(6, 0b00000000);		//
	MAX7219Send(5, 0b00000000);		//
	MAX7219Send(4, 0b00000000);		//
	MAX7219Send(3, 0b01110111);		// R
	MAX7219Send(2, 0b01001111);		// E
	MAX7219Send(1, 0b01001110);		// C
}

int valueInArray(int value, int arr[], int n)
{
	for (int i = 0; i < n; ++i) {
		if (arr[i] == value)
			return 1;
	}
	return 0;
}

void setArray(int arr[], int n, int value)
{
	for (int i = 0; i < n; ++i) {
		arr[i] = value;
	}
}
