#include <inttypes.h>
#include <stddef.h>
#include <assert.h>
/**
 * @file
 * ���λ�����ģ���ԭ�ͺͽṹ��
 */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#define RING_BUFFER_ASSERT(x) assert(x)

/**
 * ���buffer_size�Ƿ��Ƕ����ݡ�
 * ������ƣ�ֻ�� <tt> RING_BUFFER_SIZE-1 </tt> ����Ŀ
 * ���԰����ڻ������С�
 * buffer_size �����Ƕ����ݡ�
 */
#define RING_BUFFER_IS_POWER_OF_TWO(buffer_size) ((buffer_size & (buffer_size - 1)) == 0)

/**
 * ���ڱ��滺������С������
 * �Լ���������������
 */
typedef size_t ring_buffer_size_t;

/**
 * ����ȡģ������
 * ��Ϊ <tt> a % b = (a & (b - 1)) </tt>
 * ���� \c a �ǻ������е���������
 * \c b �ǻ������Ĵ�С�������ݣ���
 */
#define RING_BUFFER_MASK(rb) (rb->buffer_mask)

/**
 * �� <tt>struct ring_buffer_t</tt> ��ʹ�á�
 */
typedef struct ring_buffer_t ring_buffer_t;

/**
 * ���滷�λ������Ľṹ��
 * ����������һ����������
 * �Լ����λ�������Ԫ���ݡ�
 */
struct ring_buffer_t
{
  /** �����ڴ档 */
  char *buffer;
  /** �������롣 */
  ring_buffer_size_t buffer_mask;
  /** β������ */
  ring_buffer_size_t tail_index;
  /** ͷ������ */
  ring_buffer_size_t head_index;
};

/**
 * ��ʼ��ָ�� <em>buffer</em> �Ļ��λ�������
 * �������Ҳ���������/���û�������
 * ������������԰��� <em>buf_size-1</em> �ֽڡ�
 * @param buffer Ҫ��ʼ���Ļ��λ�������
 * @param buf Ϊ���λ���������Ļ�������
 * @param buf_size ����Ļ��λ������Ĵ�С��
 */
void ring_buffer_init(ring_buffer_t *buffer, char *buf, size_t buf_size);

/**
 * ���λ��������һ���ֽڡ�
 * @param buffer Ҫ�������ݵĻ�������
 * @param data Ҫ���õ��ֽڡ�
 */
void ring_buffer_queue(ring_buffer_t *buffer, char data);

/**
 * ���λ��������һ���ֽ����顣
 * @param buffer Ҫ�������ݵĻ�������
 * @param data ָ��Ҫ��������е��ֽ������ָ�롣
 * @param size ����Ĵ�С��
 */
void ring_buffer_queue_arr(ring_buffer_t *buffer, const char *data, ring_buffer_size_t size);

/**
 * ���ػ��λ����������ϵ��ֽڡ�
 * @param buffer Ҫ���з������ݵĻ�������
 * @param data ָ��Ӧ��������λ�õ�ָ�롣
 * @return ���������������Ϊ1������Ϊ0��
 */
uint8_t ring_buffer_dequeue(ring_buffer_t *buffer, char *data);

/**
 * ���ػ��λ����������ϵ� <em>len</em> ���ֽڡ�
 * @param buffer Ҫ���з������ݵĻ�������
 * @param data ָ��Ӧ�������ݵ������ָ�롣
 * @param len Ҫ���ص�����ֽ�����
 * @return ���ص��ֽ�����
 */
ring_buffer_size_t ring_buffer_dequeue_arr(ring_buffer_t *buffer, char *data, ring_buffer_size_t len);

/**
 * �鿴���λ�������������һ��Ԫ�ض����Ƴ�����
 * @param buffer Ҫ���з������ݵĻ�������
 * @param data ָ��Ӧ��������λ�õ�ָ�롣
 * @param index Ҫ�鿴��������
 * @return ���������������Ϊ1������Ϊ0��
 */
uint8_t ring_buffer_peek(ring_buffer_t *buffer, char *data, ring_buffer_size_t index);

/**
 * ���ػ��λ������Ƿ�Ϊ�ա�
 * @param buffer Ҫ�������Ƿ�Ϊ�յĻ�������
 * @return ���Ϊ����Ϊ1������Ϊ0��
 */
inline uint8_t ring_buffer_is_empty(ring_buffer_t *buffer)
{
  return (buffer->head_index == buffer->tail_index);
}

/**
 * ���ػ��λ������Ƿ�������
 * @param buffer Ҫ�������Ƿ������Ļ�������
 * @return ���������Ϊ1������Ϊ0��
 */
inline uint8_t ring_buffer_is_full(ring_buffer_t *buffer)
{
  return ((buffer->head_index - buffer->tail_index) & RING_BUFFER_MASK(buffer)) == RING_BUFFER_MASK(buffer);
}

/**
 * ���ػ��λ������е���Ŀ����
 * @param buffer Ҫ������Ŀ���Ļ�������
 * @return ���λ������е���Ŀ����
 */
inline ring_buffer_size_t ring_buffer_num_items(ring_buffer_t *buffer)
{
  return ((buffer->head_index - buffer->tail_index) & RING_BUFFER_MASK(buffer));
}

#endif /* RINGBUFFER_H */
