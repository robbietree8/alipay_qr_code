#include <dirent.h>

#include "bsp/esp-bsp.h"
#include "esp_log.h"

static const char *TAG = "main";

static lv_group_t *g_btn_op_group = NULL;

static void image_display(void);

void app_main(void)
{
    /* Initialize I2C (for touch and audio) */
    bsp_i2c_init();

    /* Initialize display and LVGL */
    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_H_RES * CONFIG_BSP_LCD_DRAW_BUF_HEIGHT,
        .double_buffer = 0,
        .flags = {
            .buff_dma = true,
        }
    };
    bsp_display_start_with_config(&cfg);

    /* Set display brightness to 100% */
    bsp_display_backlight_on();

    /* Mount SPIFFS */
    bsp_spiffs_mount();

    image_display();
}

const char* get_image_filename(const char* input) {
    if (strcmp(input, "alipay") == 0) {
        return "alipay.png";
    } else if (strcmp(input, "wechat") == 0) {
        return "wechat.png";
    } else {
        return "unknown.png";
    }
}

static void btn_event_cb(lv_event_t *event)
{
    lv_obj_t *img = (lv_obj_t *) event->user_data;
    const char *file_name = lv_list_get_btn_text(lv_obj_get_parent(event->target), event->target);
    char *file_name_with_path = (char *) heap_caps_malloc(256, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    if (NULL != file_name_with_path) {
        /* Get full file name with mount point and folder path */
        strcpy(file_name_with_path, "S:/spiffs/");
        strcat(file_name_with_path, get_image_filename(file_name));

        /* Set src of image with file name */
        lv_img_set_src(img, file_name_with_path);

        /* Align object */
        lv_obj_align(img, LV_ALIGN_CENTER, 0, -20);

        /* Only for debug */
        ESP_LOGI(TAG, "Display image file : %s", file_name_with_path);

        /* Don't forget to free allocated memory */
        free(file_name_with_path);
    }
}

static void image_display(void)
{
    lv_indev_t *indev = lv_indev_get_next(NULL);

    if ((lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) || \
            lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER) {
        ESP_LOGI(TAG, "Input device type is keypad");
        g_btn_op_group = lv_group_create();
        lv_indev_set_group(indev, g_btn_op_group);
    }

    lv_obj_t *img = lv_img_create(lv_scr_act());

    lv_obj_t *btn1 = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn1, 70, 30);
    lv_obj_add_event_cb(btn1, btn_event_cb, LV_EVENT_CLICKED, (void *) img);
    lv_obj_align(btn1, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_t *label1 = lv_label_create(btn1);
    lv_label_set_text(label1, "alipay");

    lv_obj_t *btn2 = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn2, 70, 30);
    lv_obj_add_event_cb(btn2, btn_event_cb, LV_EVENT_CLICKED, (void *) img);
    lv_obj_align(btn2, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_t *label2 = lv_label_create(btn2);
    lv_label_set_text(label2, "wechat");

    lv_event_send(btn1, LV_EVENT_CLICKED, NULL);
}
