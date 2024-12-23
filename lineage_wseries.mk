# Boot animation
TARGET_SCREEN_HEIGHT := 1440
TARGET_SCREEN_WIDTH := 1440
TARGET_BOOTANIMATION_HALF_RES := true

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/product_launched_with_l.mk)

# Inherit some common Lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Inherit device configuration
$(call inherit-product, device/blackberry/wseries/device.mk)

## Device identifier. This must come after all inclusions
PRODUCT_BRAND := BlackBerry
PRODUCT_DEVICE := wseries
PRODUCT_MODEL := Passport
PRODUCT_MANUFACTURER := BlackBerry
PRODUCT_NAME := lineage_wseries

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRODUCT_NAME=wseries

BUILD_FINGERPRINT := unknown/oslorow/oslo:5.1/LMY47D/AAC014:user/test-keys
