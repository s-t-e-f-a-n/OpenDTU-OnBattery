<template>
    <BasePage :title="$t('temperatureadmin.TemperatureSettings')" :isLoading="dataLoading">
        <BootstrapAlert v-model="showAlert" dismissible :variant="alertType">
            {{ alertMessage }}
        </BootstrapAlert>

        <form @submit="saveTemperatureConfig">
            <CardElement :text="$t('temperatureadmin.TemperatureConfiguration')" textVariant="text-bg-primary">
                <InputElement :label="$t('temperatureadmin.EnableTemperature')"
                              v-model="TemperatureConfigList.temperature_enabled"
                              type="checkbox" wide/>
            </CardElement>

            <CardElement :text="$t('temperatureadmin.TemperatureParameter')" textVariant="text-bg-primary" add-space
                         v-show="TemperatureConfigList.temperature_enabled"
            >
                <InputElement :label="$t('temperatureadmin.UpdatesOnly')"
                              v-model="TemperatureConfigList.temperature_updatesonly"
                              type="checkbox" wide/>
            </CardElement>

            <button type="submit" class="btn btn-primary mb-3">{{ $t('temperatureadmin.Save') }}</button>
        </form>
    </BasePage>
</template>

<script lang="ts">
import BasePage from '@/components/BasePage.vue';
import BootstrapAlert from "@/components/BootstrapAlert.vue";
import CardElement from '@/components/CardElement.vue';
import InputElement from '@/components/InputElement.vue';
import type { TemperatureConfig } from "@/types/TemperatureConfig";
import { authHeader, handleResponse } from '@/utils/authentication';
import { defineComponent } from 'vue';

export default defineComponent({
    components: {
        BasePage,
        BootstrapAlert,
        CardElement,
        InputElement,
    },
    data() {
        return {
            dataLoading: true,
            TemperatureConfigList: {} as TemperatureConfig,
            alertMessage: "",
            alertType: "info",
            showAlert: false,
        };
    },
    created() {
        this.getTemperatureConfig();
    },
    methods: {
        getTemperatureConfig() {
            this.dataLoading = true;
            fetch("/api/temperature/config", { headers: authHeader() })
                .then((response) => handleResponse(response, this.$emitter, this.$router))
                .then((data) => {
                    this.TemperatureConfigList = data;
                    this.dataLoading = false;
                });
        },
        saveTemperatureConfig(e: Event) {
            e.preventDefault();

            const formData = new FormData();
            formData.append("data", JSON.stringify(this.TemperatureConfigList));

            fetch("/api/temperature/config", {
                method: "POST",
                headers: authHeader(),
                body: formData,
            })
                .then((response) => handleResponse(response, this.$emitter, this.$router))
                .then(
                    (response) => {
                        this.alertMessage = this.$t('apiresponse.' + response.code, response.param);
                        this.alertType = response.type;
                        this.showAlert = true;
                    }
                );
        },
    },
});
</script>