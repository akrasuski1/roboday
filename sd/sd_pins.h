#ifndef SD_SD_PINS_H_
#define SD_SD_PINS_H_

#define MISO_PINX PIND
#define MISO_PIN  (1<<3)

#define SCK_DDR  DDRD
#define SCK_PORT PORTD
#define SCK_PIN  (1<<4)

#define MOSI_DDR  DDRD
#define MOSI_PORT PORTD
#define MOSI_PIN  (1<<5)

#define CS_DDR  DDRD
#define CS_PORT PORTD
#define CS_PIN  (1<<6)

#define DO_INIT()					/* Initialize port for MMC DO as input */
#define DO			(MISO_PINX & MISO_PIN)	/* Test for MMC DO ('H':true, 'L':false) */

#define DI_INIT()	MOSI_DDR  |= MOSI_PIN	/* Initialize port for MMC DI as output */
#define DI_H()		MOSI_PORT |= MOSI_PIN	/* Set MMC DI "high" */
#define DI_L()		MOSI_PORT &= ~MOSI_PIN	/* Set MMC DI "low" */

#define CK_INIT()	SCK_DDR  |= SCK_PIN	    /* Initialize port for MMC SCLK as output */
#define CK_H()		SCK_PORT |= SCK_PIN	    /* Set MMC SCLK "high" */
#define	CK_L()		SCK_PORT &= ~SCK_PIN	/* Set MMC SCLK "low" */

#define CS_INIT()	CS_DDR  |= CS_PIN	/* Initialize port for MMC CS as output */
#define	CS_H()		CS_PORT |= CS_PIN	/* Set MMC CS "high" */
#define CS_L()		CS_PORT &= ~CS_PIN	/* Set MMC CS "low" */

#endif
