#include <inttypes.h>
#include <stddef.h>
#include <assert.h>
/**
 * @file
 * 环形缓冲区模块的原型和结构。
 */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#define RING_BUFFER_ASSERT(x) assert(x)

/**
 * 检查buffer_size是否是二的幂。
 * 由于设计，只有 <tt> RING_BUFFER_SIZE-1 </tt> 个项目
 * 可以包含在缓冲区中。
 * buffer_size 必须是二的幂。
 */
#define RING_BUFFER_IS_POWER_OF_TWO(buffer_size) ((buffer_size & (buffer_size - 1)) == 0)

/**
 * 用于保存缓冲区大小的类型
 * 以及缓冲区的索引。
 */
typedef size_t ring_buffer_size_t;

/**
 * 用作取模操作符
 * 因为 <tt> a % b = (a & (b - 1)) </tt>
 * 其中 \c a 是缓冲区中的正索引，
 * \c b 是缓冲区的大小（二的幂）。
 */
#define RING_BUFFER_MASK(rb) (rb->buffer_mask)

/**
 * 简化 <tt>struct ring_buffer_t</tt> 的使用。
 */
typedef struct ring_buffer_t ring_buffer_t;

/**
 * 保存环形缓冲区的结构。
 * 缓冲区包含一个缓冲数组
 * 以及环形缓冲区的元数据。
 */
struct ring_buffer_t
{
  /** 缓冲内存。 */
  char *buffer;
  /** 缓冲掩码。 */
  ring_buffer_size_t buffer_mask;
  /** 尾索引。 */
  ring_buffer_size_t tail_index;
  /** 头索引。 */
  ring_buffer_size_t head_index;
};

/**
 * 初始化指向 <em>buffer</em> 的环形缓冲区。
 * 这个函数也可用于清空/重置缓冲区。
 * 结果缓冲区可以包含 <em>buf_size-1</em> 字节。
 * @param buffer 要初始化的环形缓冲区。
 * @param buf 为环形缓冲区分配的缓冲区。
 * @param buf_size 分配的环形缓冲区的大小。
 */
void ring_buffer_init(ring_buffer_t *buffer, char *buf, size_t buf_size);

/**
 * 向环形缓冲区添加一个字节。
 * @param buffer 要放置数据的缓冲区。
 * @param data 要放置的字节。
 */
void ring_buffer_queue(ring_buffer_t *buffer, char data);

/**
 * 向环形缓冲区添加一个字节数组。
 * @param buffer 要放置数据的缓冲区。
 * @param data 指向要放入队列中的字节数组的指针。
 * @param size 数组的大小。
 */
void ring_buffer_queue_arr(ring_buffer_t *buffer, const char *data, ring_buffer_size_t size);

/**
 * 返回环形缓冲区中最老的字节。
 * @param buffer 要从中返回数据的缓冲区。
 * @param data 指向应放置数据位置的指针。
 * @return 如果返回了数据则为1；否则为0。
 */
uint8_t ring_buffer_dequeue(ring_buffer_t *buffer, char *data);

/**
 * 返回环形缓冲区中最老的 <em>len</em> 个字节。
 * @param buffer 要从中返回数据的缓冲区。
 * @param data 指向应放置数据的数组的指针。
 * @param len 要返回的最大字节数。
 * @return 返回的字节数。
 */
ring_buffer_size_t ring_buffer_dequeue_arr(ring_buffer_t *buffer, char *data, ring_buffer_size_t len);

/**
 * 查看环形缓冲区，即返回一个元素而不移除它。
 * @param buffer 要从中返回数据的缓冲区。
 * @param data 指向应放置数据位置的指针。
 * @param index 要查看的索引。
 * @return 如果返回了数据则为1；否则为0。
 */
uint8_t ring_buffer_peek(ring_buffer_t *buffer, char *data, ring_buffer_size_t index);

/**
 * 返回环形缓冲区是否为空。
 * @param buffer 要返回其是否为空的缓冲区。
 * @return 如果为空则为1；否则为0。
 */
inline uint8_t ring_buffer_is_empty(ring_buffer_t *buffer)
{
  return (buffer->head_index == buffer->tail_index);
}

/**
 * 返回环形缓冲区是否已满。
 * @param buffer 要返回其是否已满的缓冲区。
 * @return 如果已满则为1；否则为0。
 */
inline uint8_t ring_buffer_is_full(ring_buffer_t *buffer)
{
  return ((buffer->head_index - buffer->tail_index) & RING_BUFFER_MASK(buffer)) == RING_BUFFER_MASK(buffer);
}

/**
 * 返回环形缓冲区中的项目数。
 * @param buffer 要返回项目数的缓冲区。
 * @return 环形缓冲区中的项目数。
 */
inline ring_buffer_size_t ring_buffer_num_items(ring_buffer_t *buffer)
{
  return ((buffer->head_index - buffer->tail_index) & RING_BUFFER_MASK(buffer));
}

#endif /* RINGBUFFER_H */
