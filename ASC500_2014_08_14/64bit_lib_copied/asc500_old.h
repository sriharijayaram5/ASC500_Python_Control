/*******************************************************************************
 *
 *  Project:        Daisy Client Library
 *
 *  Filename:       asc500.h
 *
 *  Purpose:        Product specific Supplement to daisybase.h for ASC500
 *
 *  Author:         NHands GmbH & Co KG
 */
/******************************************************************************/
/** @file asc500.h
 *  @brief Product specific Supplement to daisybase.h for ASC500
 *
 *  Defines constants to be used as parameters or parameter limits
 *  for the functions defined in @ref daisybase.h . They are valid when the
 *  daisybase library is used to control the Attocube ASC500 SPM controller.
 */
/******************************************************************************/
/* $Id: asc500.h,v 1.14 2010/11/22 16:37:29 trurl Exp $ */

#ifndef __ASC500_H
#define __ASC500_H


/** @brief Server Port Number
 *
 *  The TCP port number of the ASC500 application server.
 *  Parameter serverPort of @ref DYB_init .
 */
#define ASC500_PORT_NUMBER    7000

/** @brief Number of Channels
 *
 *  Number of data channels available in ASC500. The parameter channel of
 *  @ref DYB_setDataCallback has to be smaller.
 */
#define ASC500_DATA_CHANNELS  14


/** @name Global Control of Data Flow
 *
 *  The controller provides the possibility to enable / disable
 *  data channels all at once. At default the data channels are disabled.
 *  Using a XML profile with the daisybase function sendProfile() will
 *  automatically enable the data channels.
 *
 *  Only index 0 is allowed.
 *  @{
 */
#define ID_DATA_EN            0x0146  /**< Enable (1) / disable (0) all data  */
/* @} */

/** @name Configuration of Data Channels
 *
 *  The controller provides a number of data channels (@ref ASC500_DATA_CHANNELS).
 *  The four ID_CHAN_ addresses are used to configure the data that are
 *  sent on a specific channel: data source, triggering, and sampling.
 *  The ID_... constants are for "address" in control functions,
 *  CHANCONN_... and CHANADC_... are valid enumeration values for
 *  ID_CHAN_CONNECT and ID_CHAN_ADC telegrams, respectively.
 *
 *  If the scanner is selected as trigger source, the sample time can't be
 *  adjusted; it is always identical to that of the scanner.
 *
 *  The index is valid and transports the channel number of the channel
 *  to be configured.
 *  @{
 */
#define ID_CHAN_CONNECT       0x0030  /**< Data trigger (CHANCONN_-constants) */
#define ID_CHAN_ADC           0x0031  /**< Data source (CHANADC_-constants)   */
#define ID_CHAN_AVG_MAX       0x0035  /**< Average over sample time (boolean) */

/** Sample time
 *
 *  The sample time can be adjusted for timer triggered channels only
 *  (@ref CHANCONN_PERMANENT). In all other cases, it is given by the
 *  trigger source and overwritten automatically.
 *  The unit is 2.5us.
 */
#define ID_CHAN_POINTS        0x0032

/* Data Triggers (for ID_CHAN_CONNECT)                                        */
#define CHANCONN_DISABLED     0x00    /**< Channel disabled                   */
#define CHANCONN_SCANNER      0x01    /**< Triggered by scanner               */
#define CHANCONN_PERMANENT    0x02    /**< Triggered by timer                 */
#define CHANCONN_SPEC_0       0x03    /**< Triggered by Z spectroscopy engine */
#define CHANCONN_SPEC_1       0x04    /**< Triggered by DAC1 spectr engine    */
#define CHANCONN_SPEC_2       0x05    /**< Triggered by low freq spectroscopy */
#define CHANCONN_SPEC_3       0x06    /**< Triggered by calibration engine    */
#define CHANCONN_COMMAND      0x09    /**< Triggered by command               */

/* Data Sources (for ID_CHAN_ADC)                                             */
#define CHANADC_ADC_MIN        0      /**< First physical AD converter        */
#define CHANADC_ADC_MAX        5      /**< Last  physical AD converter        */
#define CHANADC_AFMAEXC        7      /**< AFM Excitation Amplitude           */
#define CHANADC_AFMFEXC        8      /**< AFM df                             */
#define CHANADC_ZOUT           9      /**< SPM Z Out                          */
#define CHANADC_AFMSIGNAL     12      /**< AFM Signal                         */
#define CHANADC_AFMAMPL       13      /**< AFM Amplitude                      */
#define CHANADC_AFMPHASE      14      /**< AFM Phase                          */
#define CHANADC_AFMMAMPL      16      /**< Lockin Amplitude                   */
#define CHANADC_AFMMPHASE     17      /**< Lockin Phase                       */
#define CHANADC_ZOUTINV       18      /**< SPM Z Out inverted                 */
#define CHANADC_ADCEXT_MIN    20      /**< First extended physical ADC        */
#define CHANADC_ADCEXT_MAX    21      /**< Last  extended physical ADC        */
/* @} */



/** @name Scanner setup
 *
 *  The addresses are used to configure the scan range and scanner sampling.
 *  The ID_... constants are for "address" in control functions,
 *  SCANRUN_... are valid enumeration values for ID_SCAN_RUNNING.
 *  The index must always be 0.
 *
 *  @ref ID_ACT_VOLT_LIM has always to be set to the limits of the piezo
 *  actuators to avoid damage.  @ref ID_ACT_GAUGE_X and ID_ACT_GAUGE_Y
 *  should be set to the actual deflection of the actuators at the voltage
 *  limit for proper calibration of all metric parameters.
 *
 *  Output activation is necessary to switch on all output voltages of the
 *  controller. The activation is asynchronous, i.e. the state requested with
 *  @ref ID_OUTPUT_ACTIVATE is not in place until acknowledged by
 *  @ref ID_OUTPUT_STATUS.
 *  @{
 */
#define ID_SCAN_X_EQ_Y        0x1006  /**< Fix no of lines = no of cols       */
#define ID_SCAN_GEOMODE       0x1004  /**< Fix aspect ratio of scanfield      */
#define ID_SCAN_OF_IN_X       0x1023  /**< Scan offset X [pm]                 */
#define ID_SCAN_OF_IN_Y       0x1024  /**< Scan offset Y [pm]                 */
#define ID_SCAN_COLUMNS       0x1003  /**< Number of scan columns             */
#define ID_SCAN_LINES         0x001D  /**< Number of scan lines               */
#define ID_SCAN_PIXEL         0x1025  /**< Size of a column/line [pm]         */
#define ID_SCAN_ROTATION      0x0018  /**< Scanfeld rotation [360/65536 deg]  */
#define ID_SCAN_MSPPX         0x1020  /**< Scanner sample time [2.5us]        */
#define ID_SCAN_RUNNING       0x0100  /**< Scanner state (SCANRUN_ constants) */
#define ID_ACT_GAUGE_X        0x1032  /**< Max actuator deflection X [pm]     */
#define ID_ACT_GAUGE_Y        0x1033  /**< Max actuator deflection Y [pm]     */
#define ID_ACT_VOLT_LIM       0x1034  /**< Max scanner output voltage         */
#define ID_OUTPUT_ACTIVATE    0x0141  /**< (de)activate outputs (boolean)     */
#define ID_OUTPUT_STATUS      0x0140  /**< If outputs are active (read only)  */

/* Scanner States (for ID_SCAN_RUNNING)                                       */
#define SCANRUN_OFF           0x00    /**< Scanner state: off                 */
#define SCANRUN_ON            0x01    /**< Scanner state: running             */
#define SCANRUN_PAUSE         0x02    /**< Scanner state: pause               */
/* @} */


/** @name Z control
 *
 *  @{
 */
#define ID_AAP_CTRL           0x0090  /**< Auto approach on/off (boolean)     */
#define ID_CRS_AXIS_UP        0x0285  /**< Coarse Axis Up by n steps          */
#define ID_CRS_AXIS_DN        0x0286  /**< Coarse Axis Down by n steps        */
#define ID_CRS_AXIS_CUP       0x0287  /**< Coarse Axis cont Up (boolean)      */
#define ID_CRS_AXIS_CDN       0x0288  /**< Coarse Axis cont Down (boolean)    */
#define ID_REG_LOOP_ON        0x0060  /**< Feedback on/off (boolean)          */
#define ID_REG_KI_DISP        0x10A3  /**< Feedback integral part             */
#define ID_REG_KP_DISP        0x10A4  /**< Feedback proportional part         */
/* @} */


/** @name Scan Offset Feedback Control
 *
 *  The addresses are used to configure the two feedback loops for scan offset.
 *  The feedback loop for offset x is accessed by index 0, feedback loop for
 *  offset y is accessed by index 1.
 *  @{
 */
#define ID_REG_GEN_CTL        0x01C6  /**< Feedback On/off (boolean)          */
#define ID_REG_GEN_POL        0x01C7  /**< Feedback polarity                  */
#define ID_REG_GEN_INPUT      0x01C8  /**< Input value (enum 0:ADC1; 1:ADC2)  */
#define ID_REG_GEN_MIN        0x01C9  /**< Feedback minimum output [uV]       */
#define ID_REG_GEN_MAX        0x01CA  /**< Feedback maximum output [uV]       */
#define ID_REG_GEN_RESET      0x01CB  /**< Feedback reset                     */
#define ID_REG_GEN_KI_DISP    0x10D0  /**< Feedback factor ki [mHz]           */
#define ID_REG_GEN_KP_DISP    0x10D1  /**< Feedback factor kp [u]             */
#define ID_REG_GEN_PI_CONST   0x10D4  /**< Forces constant P/I                */

/** Setpoint
 *
 *  The setpoint is given in the units of the input ADC, but multiplied
 *  by 10000 for better resolution. The unit can be retrieved 
 *  by @ref ID_GUI_UNIT_GENREG
 */
#define ID_REG_GEN_SP_DISP    0x10D2

/** Unit of setpoint
 *
 *  The parameter is read only, DYB_set functions will fail. It reflects
 *  the unit of the input ADC (@ref ID_REG_GEN_INPUT) and the setpoint.
 *  See @ref DataUnits "Data Units".
 */
#define ID_GUI_UNIT_GENREG    0x10D5
/* @} */


/** @name Data Units 
 *
 *  @anchor DataUnits
 *  Constants used by e.g. @ref ID_GUI_UNIT_GENREG to inform about
 *  the unit of a parameter.
 *  @{
 */
#define DATA_UNIT_MM          0x0000  /**< [mm]                               */
#define DATA_UNIT_UM          0x0001  /**< [um]                               */
#define DATA_UNIT_NM          0x0002  /**< [nm]                               */
#define DATA_UNIT_PM          0x0003  /**< [pm]                               */
#define DATA_UNIT_V           0x0004  /**< [V]                                */
#define DATA_UNIT_MV          0x0005  /**< [mV]                               */
#define DATA_UNIT_UV          0x0006  /**< [uV]                               */
#define DATA_UNIT_NV          0x0007  /**< [nV]                               */
#define DATA_UNIT_MHZ         0x0008  /**< [MHz]                              */
#define DATA_UNIT_KHZ         0x0009  /**< [kHz]                              */
#define DATA_UNIT_HZ          0x000A  /**< [Hz]                               */
#define DATA_UNIT_IHZ         0x000B  /**< [mHz]                              */
#define DATA_UNIT_S           0x000C  /**< [s]                                */
#define DATA_UNIT_MS          0x000D  /**< [ms]                               */
#define DATA_UNIT_US          0x000E  /**< [us]                               */
#define DATA_UNIT_NS          0x000F  /**< [ns]                               */
#define DATA_UNIT_A           0x0010  /**< [A]                                */
#define DATA_UNIT_MA          0x0011  /**< [mA]                               */
#define DATA_UNIT_UA          0x0012  /**< [uA]                               */
#define DATA_UNIT_NA          0x0013  /**< [nA]                               */
#define DATA_UNIT_DEG         0x0014  /**< [deg]                              */
#define DATA_UNIT_COS         0x0018  /**< [cos]                              */
#define DATA_UNIT_DB          0x001C  /**< [dB]                               */
#define DATA_UNIT_W           0x0020  /**< [W]                                */
#define DATA_UNIT_MW          0x0021  /**< [mW]                               */
#define DATA_UNIT_UW          0x0022  /**< [uW]                               */
#define DATA_UNIT_NW          0x0023  /**< [nW]                               */
/* @} */


/** @name Status Information
 *
 *  The following addresses are read only, DYB_set functions will fail.
 *  They provide information about the current process state and are sent
 *  periodically by the controller. The values can be retreived with
 *  DYB_get functions or by setting up an event callback.
 *  @{
 */
#define ID_SCAN_CURR_X        0x002A  /**< Scanner position X [funny unit]    */
#define ID_SCAN_CURR_Y        0x002B  /**< Scanner position Y [funny unit]    */
#define ID_REG_GET_Z_M        0x1038  /**< Readback z [pm]                    */
/* @} */



/** @name ADC Values
 *
 *  The following addresses are read only, DYB_set functions will fail.
 *  They provide information about the current voltage measured by the ADC
 *  selected by the index. Index=0 relates to ADC 1, Index=1 relates to
 *  ADC 2 and so on. The appropriate unit to a given ADC value is supplied
 *  by @ref ID_ADC_VAL_UNIT, see @ref DataUnits "Data Units" for details.
 *  The ADC value itself is multiplied with 1.000.000 to provide sufficent
 *  accuracy. The values can be retreived with DYB_get functions or by
 *  setting up an event callback.
 *  @{
 */
#define ID_ADC_VALUE          0x0037  /**< ADC value                          */
#define ID_ADC_VAL_UNIT       0x0038  /**< Unit of ADC value                  */
/* @} */


/** @name Server Control
 *
 *  The server's built in tracing can be controlled with these flags.
 *  By default, the tracing goes to stdout.
 *  The TRACE_... constants are flags that may be or'ed together as value
 *  for the telegram @ref ID_SRV_TRACEFLG .
 *  @{
 */
#define TRACE_FULL            0x0001 /**< Trace full length of data           */
#define TRACE_GLOBAL          0x0004 /**< Trace warnings and infos            */
#define TRACE_CS_IN           0x0008 /**< Trace telegrams from clients        */
#define TRACE_CS_OUT          0x0010 /**< Trace telegrams to clients          */
#define TRACE_UC_IN           0x0020 /**< Trace telegrams from controller     */
#define TRACE_UC_OUT          0x0040 /**< Trace telegrams to controller       */
#define TRACE_DATA            0x0080 /**< Include data telegrams in trace     */
#define TRACE_EVT             0x0100 /**< Include event telegrams in trace    */
#define ID_SRV_TRACEFLG       0x10A0 /**< Enable server tracing features      */
/* @} */


#endif
