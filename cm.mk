## Specify phone tech before including full_phone
$(call inherit-product, vendor/cm/config/gsm.mk)

# Release name
PRODUCT_RELEASE_NAME := OptimusL3

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

# Inherit device configuration
$(call inherit-product, device/lge/e400/e400.mk)

## Device identifier. This must come after all inclusions
PRODUCT_DEVICE := e400
PRODUCT_NAME := cm_e400
PRODUCT_BRAND := lge
PRODUCT_MODEL := LG-E400
PRODUCT_MANUFACTURER := LGE
