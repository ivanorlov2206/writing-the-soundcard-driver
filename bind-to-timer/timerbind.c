#include <linux/module.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <sound/timer.h>

static struct snd_timer_instance *timeri;
static struct snd_timer_id tid = {
	.dev_class = SNDRV_TIMER_CLASS_GLOBAL,
	.dev_sclass = SNDRV_TIMER_SCLASS_APPLICATION,
};

static u32 timer_interval;

static void timer_func(struct snd_timer_instance *ti, unsigned long ticks, unsigned long resolution)
{
	pr_info("Timer fire!\n %lu", ticks);
}

static void timer_event(struct snd_timer_instance *ti, int event, struct timespec64 *tstamp,
			unsigned long resolution)
{

}

static int bind_timer(void)
{
	int err;

	tid.card = -1;
	tid.device = SNDRV_TIMER_GLOBAL_HRTIMER;
	tid.subdevice = 0;
	timeri = snd_timer_instance_new("Demotimer");
	if (!timeri) {
		pr_err("Can't create the timer\n");
		return PTR_ERR(timeri);
	}
	timeri->flags |= SNDRV_TIMER_IFLG_AUTO;
	timeri->callback = timer_func;
	timeri->ccallback = timer_event;

	err = snd_timer_open(timeri, &tid, 0x1234);
	if (err) {
		pr_err("Error opening the timer: %d\n", err);
		snd_timer_instance_free(timeri);
		return err;
	}

	return 0;
}

static int timer_interval_get(void *data, u64 *val)
{
	*val = timer_interval;

	return 0;
}

static int timer_interval_set(void *data, u64 val)
{
	snd_timer_stop(timeri);
	if (!val)
		return 0;
	timer_interval = val;
	snd_timer_start(timeri, val);

	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(timer_fops, timer_interval_get, timer_interval_set, "%lld\n");
static int __init mod_init(void)
{
	int err;
	struct dentry *d;

	err = bind_timer();
	if (err) {
		pr_info("Failed to bind timer :(\n");
		return err;
	}

	d = debugfs_create_dir("bindtimer", NULL);
	debugfs_create_file("timer", S_IRUGO|S_IWUSR, d, NULL, &timer_fops);
	return 0;
}

static void __exit mod_exit(void)
{
	if (timeri)
		snd_timer_instance_free(timeri);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivan Orlov");
module_init(mod_init);
module_exit(mod_exit);
