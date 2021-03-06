uint16_t rtc_init(void);
void rtc_start(void);
void rtc_stop(void);
uint16_t rtc_write(uint8_t rtc_adresse, uint8_t rtc_wert);
uint8_t rtc_read(uint8_t rtc_adresse);
void rtc_set_time(uint8_t stunde, uint8_t minute, uint8_t sekunde);
void rtc_set_date(uint8_t jahr, uint8_t monat, uint8_t tag);
uint8_t rtc_read_sec(void);
uint8_t rtc_read_min(void);
uint8_t rtc_read_hour(void);
uint8_t rtc_read_year(void);
uint8_t rtc_read_month(void);
uint8_t rtc_read_day(void);
