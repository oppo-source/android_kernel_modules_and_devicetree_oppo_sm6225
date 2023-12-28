#ifndef __BQ25890H_HEADER__
#define __BQ25890H_HEADER__


#define SY697X_USB_VLIM            4500
#define SY697X_USB_ILIM            2000
#define SY697X_USB_VREG            4200
#define SY697X_CHG_ICHG            2000
#define SY697X_CHG_PRCHG           256
#define SY697X_CHG_TERM            250
#define SY697X_OTG_BOOTSTV         5000
#define SY697X_OTG_BOOTSTI         1200
#define SY697X_VLOT_LIMIT          4400
#define SY697X_CHG_VBUS            3300
#define SY697X_BUFF_NUM            400
#define SY697X_VALU_NUM            25
#define SY697X_ICH_1000            1000


#define SY697X_NORMAL_TEMP25    25
#define SY697X_CUR_PRECENT50    50
#define SY697X_DEFLEAT_SHIP_MODE    0x2c
extern int opchg_get_real_charger_type(void);
extern int oplus_bq25890h_enable_otg(void);
extern int oplus_bq25890h_disable_otg(void);
extern void oplus_set_prswap(bool swap);

#endif
