<template>
  <div v-show="temperatureMeterData.enabled">
    <div class="col">
      <div class="card">
        <div class="card-header text-bg-success">{{ $t('temperatureinfo.TemperatureSensor') }}</div>
        <div class="card-body card-text text-center">
          <h2>
          <!--  {{ $t('Text') }}-->
          {{ $n(temperatureMeterData.Temperature.v, 'decimal', {
            minimumFractionDigits: temperatureMeterData.Temperature.d,
            maximumFractionDigits: temperatureMeterData.Temperature.d
          }) }}
          <small class="text-muted">{{ temperatureMeterData.Temperature.u }}</small>
          </h2>
        </div>
      </div>
    </div>
  </div>
</template>

<script lang="ts">
import type { TemperatureMeter } from '@/types/LiveDataStatus';
import { defineComponent, type PropType } from 'vue';

export default defineComponent({
  props: {
    temperatureMeterData: { type: Object as PropType<TemperatureMeter>, required: true },
  },
});
</script>