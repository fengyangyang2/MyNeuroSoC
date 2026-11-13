#include <stdio.h>
#include <string.h>

typedef struct __attribute__((packed)) {
    int src_id;
    int dst_id;
    int input_val;
    int sram_idx;
} Gem5DataPacket;

#define SOC_BASE_ADDR  0x10000000
#define SOC_MODE_REG   (*(volatile int*)(SOC_BASE_ADDR + 0x0000))
#define SOC_DATA_FIFO  ((volatile Gem5DataPacket*)(SOC_BASE_ADDR + 0x0200))

static void soc_send_packet(const Gem5DataPacket *pkt)
{
    memcpy((void *)SOC_DATA_FIFO, pkt, sizeof(Gem5DataPacket));
}

int main(void)
{
    Gem5DataPacket pkt;

    printf("[My App Test] ARM 32位交叉编译环境验证成功！\n");

    SOC_MODE_REG = 0;
    printf("[ARM Bare-metal] >>> 配置 PE0 为 SNN 模式...\n");

    pkt.src_id = 0; pkt.dst_id = 0; pkt.input_val = 5; pkt.sram_idx = 0;
    soc_send_packet(&pkt);
    printf("[ARM Bare-metal] 发送数据包 (强度5)...\n");

    pkt.src_id = 0; pkt.dst_id = 3; pkt.input_val = 6; pkt.sram_idx = 0;
    soc_send_packet(&pkt);
    printf("[ARM Bare-metal] 发送数据包 (强度6)...\n");

    SOC_MODE_REG = 1;
    printf("\n[ARM Bare-metal] >>> 配置 PE0 为 ANN 模式...\n");

    pkt.src_id = 0; pkt.dst_id = 2; pkt.input_val = 8; pkt.sram_idx = 0;
    soc_send_packet(&pkt);
    printf("[ARM Bare-metal] 发送数据包 (强度8)...\n");

    printf("[ARM Bare-metal] 测试完毕，退出。\n");
    return 0;
}
