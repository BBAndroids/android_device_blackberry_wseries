/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <binder/Parcel.h>
#include <binder/IInterface.h>

extern "C" int _ZNK7android6Parcel24readParcelFileDescriptorEv(android::Parcel *thiz);

extern "C" int _ZNK7android6Parcel24readParcelFileDescriptorERi(android::Parcel *thiz, int &outCommChannel) {
    outCommChannel = -1;
    return _ZNK7android6Parcel24readParcelFileDescriptorEv(thiz);
}

extern "C" void _ZN7android10IInterface8asBinderEv(void *retval, void* self) {
    android::sp<android::IBinder> *ret = new(retval) android::sp<android::IBinder>;
    *ret = android::IInterface::asBinder((android::IInterface*)self);
}

extern "C" void _ZNK7android10IInterface8asBinderEv(void *retval, void *self) {
    android::sp<android::IBinder> *ret = new(retval) android::sp<android::IBinder>;
    *ret = android::IInterface::asBinder((android::IInterface*)self);
}
