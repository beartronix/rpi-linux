// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2016 InforceComputing
 * Author: Vinay Simha BN <simhavcs@gmail.com>
 *
 * Copyright (C) 2016 Linaro Ltd
 * Author: Sumit Semwal <sumit.semwal@linaro.org>
 */

// #include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/driver.h>

#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>

#include <drm/drm_crtc.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

static const char * const regulator_names[] = {
	"vddp",
	"iovcc"
};

struct vxt_panel {
	struct drm_panel base;
	struct mipi_dsi_device *dsi;

	struct regulator_bulk_data supplies[ARRAY_SIZE(regulator_names)];

	struct gpio_desc *reset_gpio;
	// struct backlight_device *backlight;

	const struct drm_display_mode *mode;
};

static inline struct vxt_panel *to_vxt_panel(struct drm_panel *panel)
{
	return container_of(panel, struct vxt_panel, base);
}

static int vxt_panel_init(struct vxt_panel *vxt)
{
	struct mipi_dsi_device *dsi = vxt->dsi;
	struct device *dev = &vxt->dsi->dev;
	int ret;


	dev_info(dev, "vxt_panel_init()");
	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dev_info(dev, "Resetting panel, using GPIO %d...\n", desc_to_gpio(vxt->reset_gpio));
	gpiod_set_value(vxt->reset_gpio, 0);
	msleep(20);
	gpiod_set_value(vxt->reset_gpio, 1);
	msleep(200);

	ret = mipi_dsi_dcs_soft_reset(dsi);
	if (ret < 0)
		return ret;

	usleep_range(10000, 20000);

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "failed to set exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	ret = mipi_dsi_dcs_set_pixel_format(dsi, MIPI_DCS_PIXEL_FMT_24BIT << 4);
	if (ret < 0) {
		dev_err(dev, "failed to set pixel format: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_dcs_write(dsi, MIPI_DCS_WRITE_CONTROL_DISPLAY, (u8[]){ 0x24 }, 1);
	if (ret < 0) {
		dev_err(dev, "failed to write control display: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_dcs_set_display_brightness(dsi, 0xFFFF);
	if (ret < 0) {
		dev_err(dev, "mipi_dsi_dcs_set_display_brightness() returned %d\n", ret);
		return ret;
	}


	// ret = mipi_dsi_dcs_set_column_address(dsi, 0, vxt->mode->hdisplay - 1);
	// if (ret < 0) {
	// 	dev_err(dev, "failed to set column address: %d\n", ret);
	// 	return ret;
	// }

	// ret = mipi_dsi_dcs_set_page_address(dsi, 0, vxt->mode->vdisplay - 1);
	// if (ret < 0) {
	// 	dev_err(dev, "failed to set page address: %d\n", ret);
	// 	return ret;
	// }

	// /*
	//  * BIT(5) BCTRL = 1 Backlight Control Block On, Brightness registers
	//  *                  are active
	//  * BIT(3) BL = 1    Backlight Control On
	//  * BIT(2) DD = 0    Display Dimming is Off
	//  */
	// ret = mipi_dsi_dcs_write(dsi, MIPI_DCS_WRITE_CONTROL_DISPLAY,
	// 			 (u8[]){ 0x24 }, 1);
	// if (ret < 0) {
	// 	dev_err(dev, "failed to write control display: %d\n", ret);
	// 	return ret;
	// }

	// /* CABC off */
	// ret = mipi_dsi_dcs_write(dsi, MIPI_DCS_WRITE_POWER_SAVE,
	// 			 (u8[]){ 0x00 }, 1);
	// if (ret < 0) {
	// 	dev_err(dev, "failed to set cabc off: %d\n", ret);
	// 	return ret;
	// }

	// ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	// if (ret < 0) {
	// 	dev_err(dev, "failed to set exit sleep mode: %d\n", ret);
	// 	return ret;
	// }

	// msleep(120);

	// ret = mipi_dsi_generic_write(dsi, (u8[]){0xB0, 0x00}, 2);
	// if (ret < 0) {
	// 	dev_err(dev, "failed to set mcap: %d\n", ret);
	// 	return ret;
	// }

	// mdelay(10);

	// /* Interface setting, video mode */
	// ret = mipi_dsi_generic_write(dsi, (u8[])
	// 			     {0xB3, 0x26, 0x08, 0x00, 0x20, 0x00}, 6);
	// if (ret < 0) {
	// 	dev_err(dev, "failed to set display interface setting: %d\n"
	// 		, ret);
	// 	return ret;
	// }

	// mdelay(20);

	// ret = mipi_dsi_generic_write(dsi, (u8[]){0xB0, 0x03}, 2);
	// if (ret < 0) {
	// 	dev_err(dev, "failed to set default values for mcap: %d\n"
	// 		, ret);
	// 	return ret;
	// }



	// ret = mipi_dsi_dcs_write(dsi, MIPI_DCS_WRITE_CONTROL_DISPLAY,
	// 			 (u8[]){ 0x24 }, 1);
	// if (ret < 0) {
	// 	dev_err(dev, "failed to write control display: %d\n", ret);
	// 	return ret;
	// }

	// /* CABC off */
	// ret = mipi_dsi_dcs_write(dsi, MIPI_DCS_WRITE_POWER_SAVE,
	// 			 (u8[]){ 0x00 }, 1);
	// if (ret < 0) {
	// 	dev_err(dev, "failed to set cabc off: %d\n", ret);
	// 	return ret;
	// }

	// /* Manufacturer commands, from android/arch/arm/boot/dts/qcom/dsi-panel-hx8399c-fhd-plus-video.dtsi */
	// dev_info(&dsi->dev, "Sending manufacturer init commands...\n");
	// u8 cmd[] = { 0x39,0x01,0x00,0x00,0x00,0x00,0x04,0xb9,0xff,0x83,0x99,0x39,0x01,0x00,0x00,0x00,0x00,0x02,0xd2,0x88,0x39,0x01,0x00,0x00,0x00,0x00,0x0c,0xb1,0x02,0x04,0x72,0x92,0x01,0x32,0xaa,0x11,0x11,0x52,0x57,0x39,0x01,0x00,0x00,0x00,0x00,0x10,0xb2,0x00,0x80,0x80,0xcc,0x05,0x07,0x5a,0x11,0x10,0x10,0x00,0x1e,0x70,0x03,0xd4,0x39,0x01,0x00,0x00,0x00,0x00,0x2d,0xb4,0x00,0xff,0x59,0x59,0x01,0xab,0x00,0x00,0x09,0x00,0x03,0x05,0x00,0x28,0x03,0x0b,0x0d,0x21,0x03,0x02,0x00,0x0c,0xa3,0x80,0x59,0x59,0x02,0xab,0x00,0x00,0x09,0x00,0x03,0x05,0x00,0x28,0x03,0x0b,0x0d,0x02,0x00,0x0c,0xa3,0x01,0x39,0x01,0x00,0x00,0x05,0x00,0x22,0xd3,0x00,0x0c,0x03,0x03,0x00,0x00,0x10,0x10,0x00,0x00,0x03,0x00,0x03,0x00,0x08,0x78,0x08,0x78,0x00,0x00,0x00,0x00,0x00,0x24,0x02,0x05,0x05,0x03,0x00,0x00,0x00,0x05,0x40,0x39,0x01,0x00,0x00,0x05,0x00,0x21,0xd5,0x20,0x20,0x19,0x19,0x18,0x18,0x02,0x03,0x00,0x01,0x24,0x24,0x18,0x18,0x18,0x18,0x24,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2f,0x2f,0x30,0x30,0x31,0x31,0x39,0x01,0x00,0x00,0x05,0x00,0x21,0xd6,0x24,0x24,0x18,0x18,0x19,0x19,0x01,0x00,0x03,0x02,0x24,0x24,0x18,0x18,0x18,0x18,0x20,0x20,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x2f,0x2f,0x30,0x30,0x31,0x31,0x39,0x01,0x00,0x00,0x00,0x00,0x02,0xbd,0x00,0x39,0x01,0x00,0x00,0x00,0x00,0x11,0xd8,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xba,0xaa,0xaa,0xaa,0xba,0xaa,0xaa,0x39,0x01,0x00,0x00,0x00,0x00,0x02,0xbd,0x01,0x39,0x01,0x00,0x00,0x00,0x00,0x11,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x82,0xea,0xaa,0xaa,0x82,0xea,0xaa,0xaa,0x39,0x01,0x00,0x00,0x00,0x00,0x02,0xbd,0x02,0x39,0x01,0x00,0x00,0x00,0x00,0x09,0xd8,0xff,0xff,0xc0,0x3f,0xff,0xff,0xc0,0x3f,0x39,0x01,0x00,0x00,0x00,0x00,0x02,0xbd,0x00,0x39,0x01,0x00,0x00,0x05,0x00,0x37,0xe0,0x01,0x21,0x31,0x2d,0x66,0x6f,0x7b,0x75,0x7a,0x81,0x86,0x89,0x8c,0x90,0x95,0x97,0x9a,0xa1,0xa2,0xaa,0x9e,0xad,0xb0,0x5b,0x57,0x63,0x7a,0x01,0x21,0x31,0x2d,0x66,0x6f,0x7b,0x75,0x7a,0x81,0x86,0x89,0x8c,0x90,0x95,0x97,0x9a,0xa1,0xa2,0xaa,0x9e,0xad,0xb0,0x5b,0x57,0x63,0x7a,0x39,0x01,0x00,0x00,0x00,0x00,0x03,0xb6,0x7e,0x7e,0x39,0x01,0x00,0x00,0x00,0x00,0x02,0xcc,0x08,0x05,0x01,0x00,0x00,0x96,0x00,0x02,0x11,0x00,0x05,0x01,0x00,0x00,0x32,0x00,0x02,0x29,0x00 };
	// ret = mipi_dsi_generic_write(dsi, cmd, sizeof(cmd));
	// if (ret < 0) {
	// 	dev_err(&dsi->dev, "Failed to send init command: %d\n", ret);
	// 	return ret;
	// }

	return 0;
}

static int vxt_panel_on(struct vxt_panel *vxt)
{
	struct mipi_dsi_device *dsi = vxt->dsi;
	struct device *dev = &vxt->dsi->dev;
	int ret;

	dev_info(dev, "vxt_panel_on()");

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0)
		dev_err(dev, "failed to set display on: %d\n", ret);

	return ret;
}

static void vxt_panel_off(struct vxt_panel *vxt)
{
	struct mipi_dsi_device *dsi = vxt->dsi;
	struct device *dev = &vxt->dsi->dev;
	int ret;

	dev_info(dev, "vxt_panel_off()");

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0)
		dev_err(dev, "failed to set display off: %d\n", ret);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0)
		dev_err(dev, "failed to enter sleep mode: %d\n", ret);

	msleep(100);
}

static int vxt_panel_disable(struct drm_panel *panel)
{
	struct vxt_panel *vxt = to_vxt_panel(panel);
	struct device *dev = &vxt->dsi->dev;
	// backlight_disable(vxt->backlight);

	dev_info(dev, "vxt_panel_disable()");

	return 0;
}

static int vxt_panel_unprepare(struct drm_panel *panel)
{
	struct vxt_panel *vxt = to_vxt_panel(panel);
	struct device *dev = &vxt->dsi->dev;
	int ret;

	dev_info(dev, "vxt_panel_unprepare()");

	vxt_panel_off(vxt);

	ret = regulator_bulk_disable(ARRAY_SIZE(vxt->supplies), vxt->supplies);
	if (ret < 0)
		dev_err(dev, "regulator disable failed, %d\n", ret);

	gpiod_set_value_cansleep(vxt->reset_gpio, 1);
	return 0;
}

static int vxt_panel_prepare(struct drm_panel *panel)
{
	struct vxt_panel *vxt = to_vxt_panel(panel);
	struct device *dev = &vxt->dsi->dev;
	int ret;

	dev_info(dev, "vxt_panel_prepare()");

	ret = regulator_bulk_enable(ARRAY_SIZE(vxt->supplies), vxt->supplies);
	if (ret < 0) {
		dev_err(dev, "regulator enable failed, %d\n", ret);
		return ret;
	}

	msleep(20);

	gpiod_set_value_cansleep(vxt->reset_gpio, 0);
	usleep_range(10, 20);

	ret = vxt_panel_init(vxt);
	if (ret < 0) {
		dev_err(dev, "failed to init panel: %d\n", ret);
		goto poweroff;
	}

	ret = vxt_panel_on(vxt);
	if (ret < 0) {
		dev_err(dev, "failed to set panel on: %d\n", ret);
		goto poweroff;
	}

	// u8 dispId[4] = {0,0,0,0};
	// dev_info(dev, "Read Display ID...");
	// mipi_dsi_dcs_read(vxt->dsi, MIPI_DCS_GET_DISPLAY_ID, dispId, sizeof(dispId));
	// dev_info(dev, "Display ID: 0x%08x", *(int*)dispId);

	return 0;

poweroff:
	ret = regulator_bulk_disable(ARRAY_SIZE(vxt->supplies), vxt->supplies);
	if (ret < 0)
		dev_err(dev, "regulator disable failed, %d\n", ret);

	gpiod_set_value_cansleep(vxt->reset_gpio, 1);
	return ret;
}

static int vxt_panel_enable(struct drm_panel *panel)
{
	struct vxt_panel *vxt = to_vxt_panel(panel);
	struct device *dev = &vxt->dsi->dev;
	// backlight_enable(vxt->backlight);

	dev_info(dev, "vxt_panel_enable()");

	return 0;
}

static const struct drm_display_mode default_mode = {
		.clock 			= 78745,
		.hdisplay 		= 1080,
		.hsync_start 	= 1080 + 32,
		.hsync_end 		= 1080 + 32 + 32,
		.htotal 		= 1080 + 32 + 32 + 32,
		.vdisplay 		= 1080,
		.vsync_start 	= 1080 + 16,
		.vsync_end 		= 1080 + 16 + 4,
		.vtotal 		= 1080 + 16 + 4 + 16,
		.flags 			= DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_NVSYNC,
};

static int vxt_panel_get_modes(struct drm_panel *panel,
			       struct drm_connector *connector)
{
	struct drm_display_mode *mode;
	struct vxt_panel *vxt = to_vxt_panel(panel);
	struct device *dev = &vxt->dsi->dev;

	dev_info(dev, "vxt_panel_get_modes()");

	mode = drm_mode_duplicate(connector->dev, &default_mode);
	if (!mode) {
		dev_err(dev, "failed to add mode %ux%ux@%u\n",
			default_mode.hdisplay, default_mode.vdisplay,
			drm_mode_vrefresh(&default_mode));
		return -ENOMEM;
	}

	drm_mode_set_name(mode);

	drm_mode_probed_add(connector, mode);

	connector->display_info.width_mm = 127;
	connector->display_info.height_mm = 127;

	return 1;
}

// static int dsi_dcs_bl_get_brightness(struct backlight_device *bl)
// {
// 	struct mipi_dsi_device *dsi = bl_get_data(bl);
// 	int ret;
// 	u16 brightness = bl->props.brightness;

// 	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

// 	ret = mipi_dsi_dcs_get_display_brightness(dsi, &brightness);
// 	if (ret < 0)
// 		return ret;

// 	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

// 	return brightness & 0xff;
// }

// static int dsi_dcs_bl_update_status(struct backlight_device *bl)
// {
// 	struct mipi_dsi_device *dsi = bl_get_data(bl);
// 	int ret;

// 	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

// 	ret = mipi_dsi_dcs_set_display_brightness(dsi, bl->props.brightness);
// 	if (ret < 0)
// 		return ret;

// 	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

// 	return 0;
// }

// static const struct backlight_ops dsi_bl_ops = {
// 	.update_status = dsi_dcs_bl_update_status,
// 	.get_brightness = dsi_dcs_bl_get_brightness,
// };

// static struct backlight_device *
// drm_panel_create_dsi_backlight(struct mipi_dsi_device *dsi)
// {
// 	struct device *dev = &dsi->dev;
// 	struct backlight_properties props;

// 	memset(&props, 0, sizeof(props));
// 	props.type = BACKLIGHT_RAW;
// 	props.brightness = 255;
// 	props.max_brightness = 255;

// 	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
// 					      &dsi_bl_ops, &props);
// }

static const struct drm_panel_funcs vxt_panel_funcs = {
	.disable = vxt_panel_disable,
	.unprepare = vxt_panel_unprepare,
	.prepare = vxt_panel_prepare,
	.enable = vxt_panel_enable,
	.get_modes = vxt_panel_get_modes,
};

static const struct of_device_id vxt_of_match[] = {
	{ .compatible = "vxt,vl050-108108nm", },
	{ }
};
MODULE_DEVICE_TABLE(of, vxt_of_match);

static int vxt_panel_add(struct vxt_panel *vxt)
{
	struct device *dev = &vxt->dsi->dev;
	int ret;
	unsigned int i;

	vxt->mode = &default_mode;

	for (i = 0; i < ARRAY_SIZE(vxt->supplies); i++)
		vxt->supplies[i].supply = regulator_names[i];

	ret = devm_regulator_bulk_get(dev, ARRAY_SIZE(vxt->supplies),
				      vxt->supplies);
	if (ret < 0)
		return dev_err_probe(dev, ret,
				     "failed to init regulator, ret=%d\n", ret);

	vxt->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(vxt->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(vxt->reset_gpio),
				     "cannot get reset-gpios %d\n", ret);

	// vxt->backlight = drm_panel_create_dsi_backlight(vxt->dsi);
	// if (IS_ERR(vxt->backlight))
	// 	return dev_err_probe(dev, PTR_ERR(vxt->backlight),
	// 			     "failed to register backlight %d\n", ret);

	vxt->base.prepare_prev_first = true;
	drm_panel_init(&vxt->base, &vxt->dsi->dev, &vxt_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	drm_panel_add(&vxt->base);

	return 0;
}

static void vxt_panel_del(struct vxt_panel *vxt)
{
	if (vxt->base.dev)
		drm_panel_remove(&vxt->base);
}

static int vxt_panel_probe(struct mipi_dsi_device *dsi)
{
	struct vxt_panel *vxt;
	int ret;

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO; // | MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_CLOCK_NON_CONTINUOUS;

	dev_info(&dsi->dev, "vxt_panel_probe()");

	vxt = devm_kzalloc(&dsi->dev, sizeof(*vxt), GFP_KERNEL);
	if (!vxt)
		return -ENOMEM;

	mipi_dsi_set_drvdata(dsi, vxt);

	vxt->dsi = dsi;

	ret = vxt_panel_add(vxt);
	if (ret < 0)
		return ret;

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		vxt_panel_del(vxt);
		return ret;
	}

	return 0;
}

static void vxt_panel_remove(struct mipi_dsi_device *dsi)
{
	struct vxt_panel *vxt = mipi_dsi_get_drvdata(dsi);
	int ret;

	dev_info(&dsi->dev, "vxt_panel_remove()");

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "failed to detach from DSI host: %d\n",
			ret);

	vxt_panel_del(vxt);
}

static struct mipi_dsi_driver vxt_panel_driver = {
	.driver = {
		.name = "panel-vxt-vl050-108108nm",
		.of_match_table = vxt_of_match,
	},
	.probe = vxt_panel_probe,
	.remove = vxt_panel_remove,
};
module_mipi_dsi_driver(vxt_panel_driver);

MODULE_AUTHOR("Stefan Egger <office@beartronics.at>");
MODULE_DESCRIPTION("VXT VL050-108108NM 5 inch circular display");
MODULE_LICENSE("GPL v2");
