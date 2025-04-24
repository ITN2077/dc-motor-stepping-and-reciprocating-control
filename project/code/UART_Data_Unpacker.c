/*********************************************************************************************************************
 * ��ģ�����zf_driver_uartģ��ʵ�֣������Ĵ���(uart_DEBUG)��zf_common_debug.h������
 * ��飺������~}��ͷ��}~��β�����ݰ������ݰ�ǰTAG_LENGTHλ���ڱ�ǩƥ�䣬�������Ϊ���ݽ���
 * ʵ�֣�
 *    �ж��н���ȡ���ֽ�ֱ�Ӵ��뻷�λ��������ڻ��λ������а��չ̶���ʽ�������ݰ������� UART_DEBUG_got_data[PACKET_MAX_SIZE] �У�
 *   ��ѭ������ UART_DEBUG_Ringbuffer_Processer(); �Ի��λ������е����ݴ���
 *   ���� PacketTag_Analysis(); �� PacketTag_TpDef_struct �ṹ�������еı�ǩƥ�䣬Ȼ��Թ��ر�����ֵ��
 * �÷���
 * *.�غɴ�Сͨ�� #define PACKET_MAX_SIZE �޸�
 * *.��ǩ����ͨ�� TAG_LENGTH �޸�
 * *.���λ�������Сͨ�� RINGBUFFER_SIZE �޸ģ�ֻ����2���ݴη�
 * *.PacketTag_TpDef_struct �����ṹ�����飬�洢��ǩ�����غ����Լ����ر���
 * *.���ݾ������λ��������棬���մ洢�� UART_DEBUG_got_data[] �У�ʹ��ʱֱ����
 * *.UART_DEBUG_data_packet_ready ���ǽ���������ϱ�־λ�������1
 * *.�ṩ�˶������ݽ�����ʽ
 * ע�⣺
 * *.ʹ��ʱ��Ҫ��UART_DEBUG��ʼ���������ж�
 * *.ʹ��ʱҪ ring_buffer_init(&ringbuffer_UART_DEBUG, ringbuffer_place_UART_DEBUG, RINGBUFFER_SIZE); ��ʼ�����λ������ṹ��
 * *.�жϺ�����ʹ�ùر���zf_common_debug.h �ļ��е� #define DEBUG_UART_USE_INTERRUPT    (0)����Ҫʹ��debug uart����ɵ��жϺ��������ֶ���
 * ���ӣ�main.c
#include "zf_common_headfile.h"

int test_value_1 = 0;
float test_value_2 = 0.000;

PacketTag_TpDef_struct Test_packet[] = {
    {"1", UnpackData_Handle_Int_FireWater, &test_value_1},
    {"2", UnpackData_Handle_Float_FireWater, &test_value_2},
    // ��Ӹ����ӳ���ϵ
};

int main(void)
{
    clock_init(SYSTEM_CLOCK_120M); // ��ʼ��оƬʱ�� ����Ƶ��Ϊ 120MHz
    debug_init();                  // ��ر��������������ڳ�ʼ��MPU ʱ�� ���Դ���
    uart_init(DEBUG_UART_INDEX, DEBUG_UART_BAUDRATE, DEBUG_UART_TX_PIN, DEBUG_UART_RX_PIN); // ���ڳ�ʼ��
    uart_rx_interrupt(DEBUG_UART_INDEX, ENABLE);                                            // ���������ж�
    interrupt_set_priority(UART8_IRQn, (0 << 5) || 1);                                     // ����usart3���ж����ȼ�
    ring_buffer_init(&ringbuffer_UART_DEBUG, ringbuffer_place_UART_DEBUG, RINGBUFFER_SIZE); // ���λ�������ʼ�� V1.1���
    printf_USART_DEBUG("hello,WSY!");
    while (1)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���
        UART_DEBUG_Ringbuffer_Processer();
        if (UART_DEBUG_data_packet_ready)
        {
            UART_DEBUG_data_packet_ready = false;
            PacketTag_Analysis(Test_packet, 2);
            DebugPrint(); // ������յ�����
            printf_USART_DEBUG("\r\ntestv1:%f\r\n", test_value_1);
            printf_USART_DEBUG("\r\ntestv2:%d\r\n", test_value_2);
        }
        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}
 * �޸ļ�¼
 *
 * ����            ����                             ��ע
 * 2024-03-24     Sxxx
 * 2024-03-31     Sxxx      ����V1.1����ӻ��λ�����ת�����ݣ����ٴ����жϿ���
 * 2024-04-14     Sxxx      ����V1.2�������������ڵ��߼���ʹ���������д��ڡ���DEBUG_UART���á�
 * 2024-04-21     Sxxx      ����V1.3�����DebugPrint�������ڵ��ԣ��Ż�ע��
 ********************************************************************************************************************/
#include "UART_Data_Unpacker.h"

// ������������
uint8_t UART_DEBUG_got_data[PACKET_MAX_SIZE] = {0}; // ���ڽ��յ�����
volatile bool UART_DEBUG_data_packet_ready = false; // ���ڽ��������ݰ��ı�־λ����ɽ�����1

// ���λ�����������˽��
ring_buffer_t ringbuffer_UART_DEBUG;
uint8_t ringbuffer_place_UART_DEBUG[RINGBUFFER_SIZE] = {0};


/*
* @brief ���ڳ�ʼ������
*/
void UART_DEBUG_Init(void)
{
    uart_init(DEBUG_UART_INDEX, DEBUG_UART_BAUDRATE, DEBUG_UART_TX_PIN, DEBUG_UART_RX_PIN); // ���ڳ�ʼ��
    uart_rx_interrupt(DEBUG_UART_INDEX, ENABLE);                                            // ���������ж�
    interrupt_set_priority(UART8_IRQn, (0 << 5) || 1);                                     // ����usart3���ж����ȼ�
    ring_buffer_init(&ringbuffer_UART_DEBUG, ringbuffer_place_UART_DEBUG, RINGBUFFER_SIZE); // ���λ�������ʼ�� V1.1���
}

/**
 * @brief �Ի��������������ǩƥ�������ʹ�á�
 * @param Tag_packet[]�� �ṹ�����飬���ڴ������ұ��ű�ǩ�����Ӻ��������
 * @param tag_count�� ���ұ�����
 * @note  ע������ı�ǩ����ȷ
 *
 */
void PacketTag_Analysis(PacketTag_TpDef_struct Tag_packet[], uint8_t tag_count)
{
    uint8_t i = 0;
    for (i = 0; i < tag_count; i++)
    {
        // ����ǩ�Ƿ�ƥ��
        if (strncmp(UART_DEBUG_got_data, Tag_packet[i].tag, TAG_LENGTH) == 0)
        {
            // ���ù����Ĵ�����
            Tag_packet[i].function_handler(UART_DEBUG_got_data, Tag_packet[i].value_ptr);
            return; // �ҵ�ƥ����󷵻�
        }
    }
    uint8_t error_tag[TAG_LENGTH + 1] = {0};
    strncpy(error_tag, UART_DEBUG_got_data, TAG_LENGTH);
    error_tag[TAG_LENGTH] = '\0';                             // ��ʽ�����ÿ���ֹ��
    printf_USART_DEBUG("The \"%s\" is not find.", error_tag); // ������
    // ���û���ҵ��κ�ƥ�䣬���������ﴦ�����
    // U1_printf("The \"%s\" is not find.", Tag_packet[i].tag);
}

/**
 *  @brief ʹ��fire water��ʽ����vofa+������
 *  @warning ��ʹ��Float���ͽ�������
 */
void UnpackData_Handle_Float_FireWater(const char *packet, void *value)
{
    // char numString[PACKET_MAX_SIZE];
    // strncpy(numString, packet + TAG_LENGTH, PACKET_MAX_SIZE - 1);
    *((float *)value) = atof(packet + TAG_LENGTH);
}
/**
 *  @brief ʹ��fire water��ʽ����vofa+������
 *  @warning ��ʹ��Int���ͽ�������
 */
void UnpackData_Handle_Int_FireWater(const char *packet, void *value)
{
    // char numString[PACKET_MAX_SIZE];
    // strncpy(numString, packet + TAG_LENGTH, PACKET_MAX_SIZE - 1);
    *((int *)value) = atoi(packet + TAG_LENGTH);
}
/**
 *  @brief ��ieee754��ʽ��ȡ��int16���ݣ������ű�ǩ���Ǹ߰�λ����һλ�ǵͰ�λ��
 *  @param  packet DEBUG�������ݰ�
 *  @param  int16_value int16_t�����������ڴ洢����
 *  @note  �����޷�-32100~32100�����������Ϊ ��ǩ����+3
 *  @warning ���ʹ��int16_t�������͹��ش˺���
 */
void UnpackData_Handle_Int16_IEEE754(const uint8 *packet, void *int16_value)
{
    uint8_t highByte = *(packet + TAG_LENGTH);    // ��ȡ���ֽ�
    uint8_t lowByte = *(packet + TAG_LENGTH + 1); // ��ȡ���ֽ�
    *(int16_t *)int16_value = (int16_t)((highByte << 8) | lowByte);
}

/**
 *  @brief ʹ��DEBUG_UART�����printf
 *  @param ��ʽ���ַ���
 *  @return void
 *  @warning
 *  @note ���ӣ� printf_USART_DEBUG("text:%d", 1212); ���ڽ��յ� text:1212
 */
void printf_USART_DEBUG(char *format_str, ...)
{
    uint8_t buffer[64];
    va_list arg;
    va_start(arg, format_str);
    vsprintf(buffer, format_str, arg);
    va_end(arg);
    uart_write_string(DEBUG_UART_INDEX, buffer);
}

/**
 *  @brief ������ν��յ���Ϣ���Լ���Ӧ��HEX��,����ץ��ʱ�۲�
 */
void DebugPrint(void)
{
    printf_USART_DEBUG("\r\nReturns:%s\r\n", UART_DEBUG_got_data);
    for (int i = 0; i < PACKET_MAX_SIZE; i++)
    {
        printf_USART_DEBUG("0x%02X ", UART_DEBUG_got_data[i]);
    }
    printf_USART_DEBUG("\r\n");
}

/**
 *  @brief UART_DEBUG�жϴ�����������һ�����ݰ���洢�����λ�����
 *  @note �������ݰ����շ�ʽ���ж��н��ա��������λ�������
 *  @warning ��isr.h����������isr.c��Ӧ���ں�����
 */
void USART_DEBUG_IRQ_Function(void)
{
    uint8_t byte = 0;
    if (uart_query_byte(DEBUG_UART_INDEX, &byte))
    {
        ring_buffer_queue(&ringbuffer_UART_DEBUG, byte); // ���ֽڷ��뻷�λ�����
                                                         // ���������ڻ������У���������ѭ���������ط�����
    }
    /*------------------------------�������ж��н������ݰ�------------------------------*/
    //
    // static uint8_t UART_DEBUG_got_data_index = 0;
    // static uint8_t prev_byte = 0;
    // static bool start_load_packet_flag = false;
    // uint8_t byte = 0;
    // if (uart_query_byte(DEBUG_UART_INDEX, &byte))
    // {
    //     if (!start_load_packet_flag)
    //     {
    //         if (prev_byte == '~' && byte == '}')
    //         {
    //             start_load_packet_flag = true;
    //             UART_DEBUG_got_data_index = 0;
    //         }
    //     }
    //     else
    //     {
    //         if (prev_byte == '}' && byte == '~' && start_load_packet_flag)
    //         {
    //             UART_DEBUG_got_data[UART_DEBUG_got_data_index - 1] = '\0';
    //             UART_DEBUG_data_packet_ready = true;
    //             start_load_packet_flag = false;
    //         }
    //         else if (UART_DEBUG_got_data_index < PACKET_MAX_SIZE)
    //         {
    //             UART_DEBUG_got_data[UART_DEBUG_got_data_index++] = byte;
    //         }
    //         else
    //         {
    //             memset(UART_DEBUG_got_data, 0, PACKET_MAX_SIZE);
    //             printf_USART_DEBUG("Error: Data packet overflow.\r\n");
    //             start_load_packet_flag = false;
    //         }
    //     }
    //     prev_byte = byte;
    // }
}

/**
 *  @brief ��UART3���λ������еı�������
 *  @note ���յ�һ���������ݰ��Ժ�UART_DEBUG_data_packet_ready ��1�����˳���ֱ�� UART_DEBUG_data_packet_ready ��0
 *  @warning UART_DEBUG_data_packet_ready ʹ�����Ժ�һ��Ҫ�����0
 */
void UART_DEBUG_Ringbuffer_Processer(void)
{
    static uint8_t UART_DEBUG_got_data_index = 0;
    static uint8_t prev_byte = 0;
    static bool start_load_packet_flag = false;
    uint8_t data = 0;

    while (!ring_buffer_is_empty(&ringbuffer_UART_DEBUG) && !UART_DEBUG_data_packet_ready)
    {
        ring_buffer_dequeue(&ringbuffer_UART_DEBUG, &data); // �ӻ�������ȡ��һ���ֽ�

        if (!start_load_packet_flag)
        {
            if (prev_byte == '~' && data == '}')
            {
                start_load_packet_flag = true;
                UART_DEBUG_got_data_index = 0;
            }
        }
        else
        {
            if (prev_byte == '}' && data == '~' && start_load_packet_flag)
            {
                UART_DEBUG_got_data[UART_DEBUG_got_data_index - 1] = '\0'; // һ��Ҫȷ��UART_DEBUG_got_data�Ѿ������Ҵ�С�㹻
                UART_DEBUG_data_packet_ready = 1;
                start_load_packet_flag = false;
                break;
            }
            else if (UART_DEBUG_got_data_index < PACKET_MAX_SIZE)
            {
                UART_DEBUG_got_data[UART_DEBUG_got_data_index++] = data;
            }
            else
            {
                memset(UART_DEBUG_got_data, 0, PACKET_MAX_SIZE);
                printf_USART_DEBUG("Error: Data packet overflow.\r\n");
                for (int i = 0; i < PACKET_MAX_SIZE; i++)
                {
                    printf_USART_DEBUG("0x%02X ", UART_DEBUG_got_data[i]);
                }
                start_load_packet_flag = false;
            }
        }
        prev_byte = data;
    }
}
